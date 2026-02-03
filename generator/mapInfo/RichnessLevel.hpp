#pragma once

#include "../global/Global.hpp"

enum class RichnessLevel { LOW = 0, MEDIUM, HIGH, UNKNOWN };

inline RichnessLevel decodeRichnessLevel(const string &richness) {
    if (richness == "low")
        return RichnessLevel::LOW;
    if (richness == "medium")
        return RichnessLevel::MEDIUM;
    if (richness == "high")
        return RichnessLevel::HIGH;
    return RichnessLevel::UNKNOWN;
}
