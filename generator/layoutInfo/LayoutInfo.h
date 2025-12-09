#pragma once

#include <bits/stdc++.h>
#include "../global/Random.h"
#include <nlohmann/json.hpp> 

using json = nlohmann::json;

class LayoutInfo {
public:

    LayoutInfo();

    void deserialize(const json& layout);
    void printLayout();

    template <typename T>
    static T getOrError(const nlohmann::json& j, const std::string& key);

private:
    std::string name;
    std::string description;
    std::string difficulty; 
    std::string mapSize;

};

template <typename T>
T LayoutInfo::getOrError(const nlohmann::json& j, const std::string& key) {
     if (j.contains(key)) { // Check if the key exists
        try {
            return j.at(key).get<T>(); // Safely attempt to get the value
        } catch (const std::exception& e) {
            throw std::runtime_error("Error reading '" + key + "': " + e.what());
        }
    }
    throw std::runtime_error("Key '" + key + "' does not exist in the JSON object.");
}