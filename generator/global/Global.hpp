#pragma once

#include <bits/stdc++.h>
#include <nlohmann/json.hpp>

#include "./float3.hpp"
#include "./int3.hpp"

using namespace std;
using json = nlohmann::json;

template <typename T> T getOrError(const nlohmann::json &j, const string &key) {
    if (j.contains(key)) { // Check if the key exists
        try {
            return j.at(key).get<T>(); // Safely attempt to get the value
        } catch (const exception &e) {
            throw runtime_error("Error reading '" + key + "': " + e.what());
        }
    }
    throw runtime_error("Key '" + key + "' does not exist in the JSON object.");
}
