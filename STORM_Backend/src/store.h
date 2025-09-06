#pragma once

#include <string>
#include <unordered_map>
#include <list>
#include <vector>
#include <mutex>
#include <memory>

/**
 * @brief Thread-safe in-memory key-value store with per-shard LRU eviction.
 *
 * Features:
 *  - Sharding: divides store into multiple independent shards to reduce mutex contention.
 *  - Single-key operations: put, get, del.
 *  - Batch operations: insert multiple key-value pairs efficiently per shard.
 *  - LRU eviction per shard: evicts least recently used key when capacity is exceeded.
 */
class Store {
public:
    /**
     * @brief Construct a new Store object with shard configuration.
     * @param maxKeysPerShard Maximum number of key-value pairs per shard.
     * @param totalShardCount Total number of independent shards in the store.
     *
     * Each shard maintains its own LRU list, hash map, and mutex to allow
     * concurrent access with minimal contention.
     */
    explicit Store(size_t maxKeysPerShard = 100, size_t totalShardCount = 16);

    // ========================================
    // Single-key operations
    // ========================================

    /**
     * @brief Insert or update a key-value pair in the store.
     * @param key Key to insert or update.
     * @param value Value to associate with the key.
     * @return true Always returns true after operation.
     */
    bool put(const std::string& key, const std::string& value);

    /**
     * @brief Retrieve the value for a given key from the store.
     * @param key Key to retrieve.
     * @param value Output parameter for the value associated with the key.
     * @return true If key exists and value was retrieved.
     * @return false If key does not exist.
     */
    bool get(const std::string& key, std::string& value);

    /**
     * @brief Delete a key-value pair from the store.
     * @param key Key to delete.
     * @return true If the key existed and was deleted.
     * @return false If the key does not exist.
     */
    bool del(const std::string& key);

    // ========================================
    // Batch operations
    // ========================================

    /**
     * @brief Insert multiple key-value pairs efficiently.
     * @param keyValuePairs Vector of key-value pairs to insert.
     *
     * Groups keys by shard to minimize lock acquisitions. Each shard is
     * locked once per batch.
     */
    void putMany(const std::vector<std::pair<std::string, std::string>>& keyValuePairs);

    // ========================================
    // Utility operations
    // ========================================

    /**
     * @brief Clear all key-value pairs from every shard in the store.
     */
    void clear();

    /**
     * @brief Print the contents of all shards to stdout for debugging.
     */
    void list();

private:
    // ========================================
    // Internal types
    // ========================================

    /**
     * @brief Represents a single key-value entry stored within a shard.
     */
    struct Entry {
        std::string value;                         ///< The value associated with the key
        std::list<std::string>::iterator recencyIt;///< Iterator into shard's recency list
    };

    /**
     * @brief Represents a shard, which stores part of the overall key-value store.
     *
     * Each shard maintains:
     *  - A hash map for key-value storage
     *  - A recency list for LRU eviction
     *  - A mutex to allow concurrent safe access
     */
    struct Shard {
        std::unordered_map<std::string, Entry> store; ///< Map from key to entry
        std::list<std::string> recencyList;           ///< Keys ordered by recency (front = most recent)
        std::mutex shardLock;                         ///< Mutex to protect shard
        size_t capacity = 0;                          ///< Maximum number of entries in this shard
    };

    // ========================================
    // Internal members
    // ========================================

    std::vector<std::unique_ptr<Shard>> shards; ///< Vector of shards (unique_ptr avoids copy/mutex issues)

    // ========================================
    // Per-shard helper functions
    // ========================================

    /**
     * @brief Insert or update a key-value pair within a specific shard.
     * @param targetShard Shard to perform operation on.
     * @param key Key to insert/update.
     * @param value Value associated with key.
     * @return true Always returns true after operation.
     */
    bool putInShard(Shard& targetShard, const std::string& key, const std::string& value);

    /**
     * @brief Retrieve a value from a specific shard.
     * @param targetShard Shard to search.
     * @param key Key to retrieve.
     * @param value Output parameter for the value.
     * @return true If key exists.
     * @return false If key does not exist.
     */
    bool getFromShard(Shard& targetShard, const std::string& key, std::string& value);

    /**
     * @brief Delete a key from a specific shard.
     * @param targetShard Shard to operate on.
     * @param key Key to delete.
     * @return true If key existed and was deleted.
     * @return false If key does not exist.
     */
    bool delFromShard(Shard& targetShard, const std::string& key);

    // ========================================
    // Shard selection helper
    // ========================================

    /**
     * @brief Determine which shard a key belongs to.
     * @param key Key to map.
     * @return size_t Index of the shard in the shards vector.
     *
     * Uses a standard hash function modulo the number of shards.
     */
    size_t shardIndex(const std::string& key) const {
        return std::hash<std::string>{}(key) % shards.size();
    }
};
