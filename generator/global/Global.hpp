#pragma once

#include <bits/stdc++.h>
#include <nlohmann/json.hpp>

#include "./float3.hpp"
#include "./int3.hpp"

using namespace std;
using json = nlohmann::json;

template <typename T> T getOrError(const nlohmann::json &j, const string &key) {
    if (j.contains(key)) {
        try {
            return j.at(key).get<T>();
        } catch (const exception &e) {
            throw runtime_error("Error reading '" + key + "': " + e.what());
        }
    }
    throw runtime_error("Key '" + key + "' does not exist in the JSON object.");
}

template <typename T>
T getOrDefault(const nlohmann::json &j, const string &key, const T &defaultValue) {
    if (j.contains(key)) {
        try {
            return j.at(key).get<T>();
        } catch (const exception &e) {
            return defaultValue;
        }
    }
    return defaultValue;
}

int3 const directions4[] = {
    // TODO think about 8 directions
    int3(1, 0, 0),
    int3(0, 1, 0),
    int3(-1, 0, 0),
    int3(0, -1, 0),
    // int3(1,1,0),
    // int3(-1,-1,0),
    // int3(-1,1,0),
    // int3(1,-1,0),
};

int3 const directions8[] = {int3(1, 0, 0), int3(0, 1, 0),   int3(-1, 0, 0), int3(0, -1, 0),
                            int3(1, 1, 0), int3(-1, -1, 0), int3(-1, 1, 0), int3(1, -1, 0)};
