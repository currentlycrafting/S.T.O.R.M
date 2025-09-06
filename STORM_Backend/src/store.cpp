#include "store.h"
#include <iostream>
#include <algorithm>
#include <memory>

// ========================================
// Constructor
// ========================================

/**
 * @brief Construct a new Store object with multiple shards.
 * @param shard_capacity Maximum number of key-value pairs per shard.
 * @param total_shards Total number of independent shards to create.
 * 
 * Each shard maintains its own LRU list, hash map, and mutex to
 * reduce lock contention in concurrent operations.
 */
Store::Store(size_t shard_capacity, size_t total_shards) {
    // Reserve space for all shards to avoid repeated allocations
    shards.reserve(total_shards);

    // Initialize each shard with its capacity
    for (size_t shard_index = 0; shard_index < total_shards; ++shard_index) {
        auto shard_pointer = std::make_unique<Shard>();
        shard_pointer->capacity = shard_capacity;
        shards.push_back(std::move(shard_pointer));
    }
}

// ========================================
// Single-key operations
// ========================================

/**
 * @brief Insert or update a key-value pair in the store.
 * @param key Key to insert/update.
 * @param value Value to associate with the key.
 * @return true Always returns true after operation.
 */
bool Store::put(const std::string& key, const std::string& value) {
    Shard& target_shard = *shards[shardIndex(key)];

    // Lock the shard to ensure thread safety
    std::lock_guard<std::mutex> shard_lock_guard(target_shard.shardLock);

    // Perform the insertion/update in the shard
    return putInShard(target_shard, key, value);
}

/**
 * @brief Retrieve the value for a given key from the store.
 * @param key Key to retrieve.
 * @param value Output parameter to store the associated value.
 * @return true If key exists and value is retrieved.
 * @return false If key does not exist.
 */
bool Store::get(const std::string& key, std::string& value) {
    Shard& target_shard = *shards[shardIndex(key)];
    std::lock_guard<std::mutex> shard_lock_guard(target_shard.shardLock);

    return getFromShard(target_shard, key, value);
}

/**
 * @brief Delete a key-value pair from the store.
 * @param key Key to delete.
 * @return true If key existed and was deleted.
 * @return false If key does not exist.
 */
bool Store::del(const std::string& key) {
    Shard& target_shard = *shards[shardIndex(key)];
    std::lock_guard<std::mutex> shard_lock_guard(target_shard.shardLock);

    return delFromShard(target_shard, key);
}

// ========================================
// Per-shard operations
// ========================================

/**
 * @brief Insert or update a key-value pair within a specific shard.
 * @param shard Target shard to perform the operation.
 * @param key Key to insert/update.
 * @param value Value to associate with the key.
 * @return true Always returns true after operation.
 * 
 * Handles LRU eviction if shard exceeds capacity.
 */
bool Store::putInShard(Shard& shard, const std::string& key, const std::string& value) {
    auto entry_iterator = shard.store.find(key);

    // Key exists: update value and move to front of LRU
    if (entry_iterator != shard.store.end()) {
        entry_iterator->second.value = value;
        shard.recencyList.splice(
            shard.recencyList.begin(),
            shard.recencyList,
            entry_iterator->second.recencyIt
        );
        entry_iterator->second.recencyIt = shard.recencyList.begin();
    } 
    // Key does not exist: insert new entry
    else {
        // Evict least recently used item if shard capacity exceeded
        if (shard.store.size() >= shard.capacity) {
            const std::string& lru_key = shard.recencyList.back();
            shard.store.erase(lru_key);
            shard.recencyList.pop_back();
        }

        // Insert new key at front of LRU list
        shard.recencyList.push_front(key);
        shard.store.emplace(key, Entry{value, shard.recencyList.begin()});
    }

    return true;
}

/**
 * @brief Retrieve the value for a key within a specific shard.
 * @param shard Target shard.
 * @param key Key to retrieve.
 * @param value Output parameter for value.
 * @return true If key exists and value retrieved.
 * @return false If key does not exist.
 * 
 * Updates recency list on access to maintain LRU ordering.
 */
bool Store::getFromShard(Shard& shard, const std::string& key, std::string& value) {
    auto entry_iterator = shard.store.find(key);

    if (entry_iterator == shard.store.end()) {
        return false;
    }

    // Move key to front of LRU list to mark it as recently used
    shard.recencyList.splice(
        shard.recencyList.begin(),
        shard.recencyList,
        entry_iterator->second.recencyIt
    );
    entry_iterator->second.recencyIt = shard.recencyList.begin();

    value = entry_iterator->second.value;
    return true;
}

/**
 * @brief Delete a key-value pair from a specific shard.
 * @param shard Target shard.
 * @param key Key to delete.
 * @return true If key existed and was deleted.
 * @return false If key does not exist.
 */
bool Store::delFromShard(Shard& shard, const std::string& key) {
    auto entry_iterator = shard.store.find(key);

    if (entry_iterator == shard.store.end()) {
        return false;
    }

    shard.recencyList.erase(entry_iterator->second.recencyIt);
    shard.store.erase(entry_iterator);
    return true;
}

// ========================================
// Batch operations
// ========================================

/**
 * @brief Insert multiple key-value pairs into the store efficiently.
 * @param key_value_pairs Vector of key-value pairs to insert.
 * 
 * Groups keys by shard to minimize lock acquisitions.
 */
void Store::putMany(const std::vector<std::pair<std::string, std::string>>& key_value_pairs) {
    std::vector<std::vector<std::pair<std::string, std::string>>> shard_batches(shards.size());

    // Assign keys to their respective shard batches
    for (const auto& key_value_pair : key_value_pairs) {
        size_t target_shard_index = shardIndex(key_value_pair.first);
        shard_batches[target_shard_index].push_back(key_value_pair);
    }

    // Insert batch into each shard while holding a single lock per shard
    for (size_t shard_index = 0; shard_index < shards.size(); ++shard_index) {
        if (shard_batches[shard_index].empty()) continue;

        Shard& target_shard = *shards[shard_index];
        std::lock_guard<std::mutex> shard_lock_guard(target_shard.shardLock);

        for (const auto& key_value_pair : shard_batches[shard_index]) {
            putInShard(target_shard, key_value_pair.first, key_value_pair.second);
        }
    }
}

// ========================================
// Utility operations
// ========================================

/**
 * @brief Clear all shards, removing every key-value pair.
 */
void Store::clear() {
    for (auto& shard_pointer : shards) {
        Shard& shard = *shard_pointer;
        std::lock_guard<std::mutex> shard_lock_guard(shard.shardLock);

        shard.store.clear();
        shard.recencyList.clear();
    }
}

/**
 * @brief Print the contents of all shards for debugging.
 */
void Store::list() {
    for (size_t shard_index = 0; shard_index < shards.size(); ++shard_index) {
        Shard& shard = *shards[shard_index];
        std::lock_guard<std::mutex> shard_lock_guard(shard.shardLock);

        std::cout << "{ \"shard_" << shard_index << "\": {\n";

        for (auto it = shard.recencyList.begin(); it != shard.recencyList.end(); ++it) {
            const std::string& key = *it;
            const Entry& entry = shard.store.at(key);

            std::cout << "  \"" << key << "\": \"" << entry.value << "\"";
            if (std::next(it) != shard.recencyList.end()) {
                std::cout << ",";
            }
            std::cout << "\n";
        }

        std::cout << "} }\n";
    }
}
