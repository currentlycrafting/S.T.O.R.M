#include "store.h"
#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>
#include <random>

/**
 * ==============================
 * Basic Operations
 * ==============================
 */

/**
 * @brief Tests inserting a key-value pair and retrieving it.
 */
TEST(StoreTest, PutAndGet) {
    Store testStore(3);

    testStore.put("foo", "bar");

    std::string retrievedValue;
    EXPECT_TRUE(testStore.get("foo", retrievedValue));
    EXPECT_EQ(retrievedValue, "bar");
}

/**
 * @brief Tests overwriting an existing key with a new value.
 */
TEST(StoreTest, OverwriteValue) {
    Store testStore(3);

    testStore.put("foo", "bar");
    testStore.put("foo", "baz"); // Overwrite value

    std::string retrievedValue;
    EXPECT_TRUE(testStore.get("foo", retrievedValue));
    EXPECT_EQ(retrievedValue, "baz");
}

/**
 * @brief Tests deleting a key from the store.
 */
TEST(StoreTest, DeleteKey) {
    Store testStore(3);

    testStore.put("foo", "bar");
    EXPECT_TRUE(testStore.del("foo"));

    std::string retrievedValue;
    EXPECT_FALSE(testStore.get("foo", retrievedValue));
}

/**
 * ==============================
 * LRU-Specific Behavior
 * ==============================
 */

/**
 * @brief Tests that the least recently used (LRU) key is evicted when capacity is exceeded.
 */
TEST(StoreTest, EvictLeastRecentlyUsed) {
    Store testStore(2, 1);  // Single shard, capacity 2

    testStore.put("A", "1");
    testStore.put("B", "2");
    testStore.put("C", "3"); // Should evict "A"

    std::string retrievedValue;
    EXPECT_FALSE(testStore.get("A", retrievedValue));
    EXPECT_TRUE(testStore.get("B", retrievedValue));
    EXPECT_EQ(retrievedValue, "2");
    EXPECT_TRUE(testStore.get("C", retrievedValue));
    EXPECT_EQ(retrievedValue, "3");
}

/**
 * @brief Tests that accessing a key updates its recency.
 */
TEST(StoreTest, RecencyUpdatesOnGet) {
    Store testStore(2, 1); // Single shard

    testStore.put("A", "1");
    testStore.put("B", "2");

    std::string retrievedValue;
    EXPECT_TRUE(testStore.get("A", retrievedValue));
    EXPECT_EQ(retrievedValue, "1");

    testStore.put("C", "3"); // Should evict "B"

    EXPECT_TRUE(testStore.get("A", retrievedValue));
    EXPECT_EQ(retrievedValue, "1");
    EXPECT_FALSE(testStore.get("B", retrievedValue));
    EXPECT_TRUE(testStore.get("C", retrievedValue));
    EXPECT_EQ(retrievedValue, "3");
}

/**
 * @brief Tests that updating a key via PUT refreshes its recency.
 */
TEST(StoreTest, RecencyUpdatesOnPut) {
    Store testStore(2, 1); // Single shard

    testStore.put("A", "1");
    testStore.put("B", "2");

    testStore.put("A", "10"); // Refresh recency

    testStore.put("C", "3"); // Should evict "B"

    std::string retrievedValue;
    EXPECT_TRUE(testStore.get("A", retrievedValue));
    EXPECT_EQ(retrievedValue, "10");
    EXPECT_FALSE(testStore.get("B", retrievedValue));
    EXPECT_TRUE(testStore.get("C", retrievedValue));
    EXPECT_EQ(retrievedValue, "3");
}

/**
 * @brief Tests that clearing the store removes all keys.
 */
TEST(StoreTest, ClearRemovesAllEntries) {
    Store testStore(3);

    testStore.put("X", "100");
    testStore.put("Y", "200");
    testStore.put("Z", "300");

    testStore.clear();

    std::string retrievedValue;
    EXPECT_FALSE(testStore.get("X", retrievedValue));
    EXPECT_FALSE(testStore.get("Y", retrievedValue));
    EXPECT_FALSE(testStore.get("Z", retrievedValue));
}

/**
 * ==============================
 * Concurrency Stress Test
 * ==============================
 */

/**
 * @brief Tests store under high concurrency with multiple threads performing mixed operations.
 */
TEST(StoreTest, ConcurrencyStress) {
    const size_t kNumShards = 30;            // Number of independent shards
    const size_t kShardCapacity = 4000;      // Maximum keys per shard
    Store testStore(kShardCapacity, kNumShards);

    const int kNumThreads = 350;             // Total concurrent worker threads
    const int kOpsPerThread = 300000;        // Operations per thread (~105M total operations)

    std::atomic<int> operationCounter{0};

    // Prepare fixed set of keys for operations
    std::vector<std::string> keyPool;
    for (int index = 0; index < 100; ++index) {
        keyPool.push_back("key_" + std::to_string(index));
    }

    // Worker function for threads performing random operations
    auto workerFunction = [&testStore, &keyPool, &operationCounter, kOpsPerThread]() {
        std::mt19937 randomGenerator(operationCounter.fetch_add(1));
        std::uniform_int_distribution<int> keyIndexDistribution(0, keyPool.size() - 1);
        std::uniform_int_distribution<int> operationDistribution(0, 2); // 0: PUT, 1: GET, 2: DEL

        for (int operationIndex = 0; operationIndex < kOpsPerThread; ++operationIndex) {
            int selectedOperation = operationDistribution(randomGenerator);
            const std::string& selectedKey = keyPool[keyIndexDistribution(randomGenerator)];
            int operationId = operationCounter.fetch_add(1);

            if (selectedOperation == 0) { // PUT operation
                if (operationId % 10 == 0) {
                    testStore.putMany({{selectedKey, "val_" + std::to_string(operationId)}});
                } else {
                    testStore.put(selectedKey, "val_" + std::to_string(operationId));
                }
            } else if (selectedOperation == 1) { // GET operation
                std::string retrievedValue;
                testStore.get(selectedKey, retrievedValue);
            } else { // DEL operation
                testStore.del(selectedKey);
            }
        }
    };

    // Launch threads
    std::vector<std::thread> workerThreads;
    for (int threadIndex = 0; threadIndex < kNumThreads; ++threadIndex) {
        workerThreads.emplace_back(workerFunction);
    }

    // Wait for all threads to finish
    for (auto& workerThread : workerThreads) {
        workerThread.join();
    }

    // Verify total number of keys does not exceed shard capacity
    std::string dummyValue;
    int totalKeysStored = 0;
    for (const auto& key : keyPool) {
        if (testStore.get(key, dummyValue)) {
            totalKeysStored++;
        }
    }

    EXPECT_LE(totalKeysStored, static_cast<int>(kNumShards * kShardCapacity));
}
