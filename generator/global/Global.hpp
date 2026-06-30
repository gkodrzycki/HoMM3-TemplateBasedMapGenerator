#pragma once

#include <bits/stdc++.h>
#include <magic_enum.hpp>
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

template <typename T> string getEnumName(T value) {
    string_view sv = magic_enum::enum_name(value);
    return string{sv};
}

template <typename T> T getEnumFromNameOrThrow(const string &name) {
    string normalized = name;
    replace(normalized.begin(), normalized.end(), ' ', '_');

    if (auto v = magic_enum::enum_cast<T>(normalized, magic_enum::case_insensitive)) {
        return *v;
    }
    throw runtime_error("Unknown enum name: " + name);
}

inline void printNonZeroFields(const string &label,
                               std::initializer_list<std::pair<const char *, int>> items) {
    bool any = false;
    for (const auto &p : items) {
        if (p.second) {
            any = true;
            break;
        }
    }
    if (!any)
        return;
    cerr << "      " << label << ":\n";
    for (const auto &p : items) {
        if (p.second)
            cerr << "       " << p.first << "=" << p.second << "\n";
    }
}

const string RED            = "\033[31m";
const string GREEN          = "\033[32m";
const string YELLOW         = "\033[33m";
const string BLUE           = "\033[34m";
const string MAGENTA        = "\033[35m";
const string CYAN           = "\033[36m";
const string WHITE          = "\033[37m";
const string BRIGHT_RED     = "\033[91m";
const string BRIGHT_GREEN   = "\033[92m";
const string BRIGHT_YELLOW  = "\033[93m";
const string BRIGHT_BLUE    = "\033[94m";
const string BRIGHT_MAGENTA = "\033[95m";
const string BRIGHT_CYAN    = "\033[96m";
const string BOLD           = "\033[1m";
const string RESET          = "\033[0m";

const auto getZoneColor = [](int zoneID) -> string {
    const string colors[] = {BRIGHT_CYAN, BRIGHT_MAGENTA, BRIGHT_GREEN, BRIGHT_YELLOW,
                             CYAN,        MAGENTA,        BLUE,         GREEN};
    return colors[zoneID % 8];
};

template <typename T> void printColor(const string &color, const T message) {
    cerr << color << message << RESET;
}

const int3 directions4[] = {
    int3(1, 0, 0),
    int3(0, 1, 0),
    int3(-1, 0, 0),
    int3(0, -1, 0),
};

const int3 directions8[] = {int3(1, 0, 0), int3(0, 1, 0),   int3(-1, 0, 0), int3(0, -1, 0),
                            int3(1, 1, 0), int3(-1, -1, 0), int3(-1, 1, 0), int3(1, -1, 0)};

inline bool isInside(int startWidth, int startHeight, int endWidth, int endHeight, int3 position) {
    return !(position.x < startWidth || position.y < startHeight || position.x >= endWidth ||
             position.y >= endHeight);
}
