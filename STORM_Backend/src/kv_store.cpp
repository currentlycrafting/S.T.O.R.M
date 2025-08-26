#include "kv_store.h"
#include <iostream>

bool KVStore::put(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mu_);
    store_[key] = value;
    std::cout << "Successfully wrote key: " << key << std::endl;
    return true;
}

bool KVStore::get(const std::string& key, std::string* value) {
    std::lock_guard<std::mutex> lock(mu_);
    auto it = store_.find(key);
    if (it != store_.end()) {
        *value = it->second;
        std::cout << "Successfully read key: " << key << std::endl;
        return true;
    }
    std::cout << "Key not found: " << key << std::endl;
    return false;
}

bool KVStore::del(const std::string& key) {
    std::lock_guard<std::mutex> lock(mu_);
    bool success = store_.erase(key) > 0;
    if (success) {
        std::cout << "Successfully deleted key: " << key << std::endl;
    } else {
        std::cout << "Key not found for deletion: " << key << std::endl;
    }
    return success;
}
