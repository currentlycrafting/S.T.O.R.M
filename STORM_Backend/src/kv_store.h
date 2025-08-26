#pragma once

#include <string>
#include <unordered_map>
#include <mutex>

class KVStore {
public:
    bool put(const std::string& key, const std::string& value);
    bool get(const std::string& key, std::string* value);
    bool del(const std::string& key);

private:
    std::unordered_map<std::string, std::string> store_;
    std::mutex mu_;
};
