#pragma once

#include "../global/Global.hpp"

enum class ResourceType {
    RESOURCE_WOOD = 0,
    RESOURCE_SULFUR,
    RESOURCE_CRYSTAL,
    RESOURCE_GEMS,
    RESOURCE_ORE,
    RESOURCE_MERCURY,
    RESOURCE_GOLD,
    RESOURCE_UNKNOWN
};

inline string resourceTypeToString(ResourceType resourceType) {
    switch (resourceType) {
    case ResourceType::RESOURCE_WOOD:
        return "RESOURCE_WOOD";
    case ResourceType::RESOURCE_SULFUR:
        return "RESOURCE_SULFUR";
    case ResourceType::RESOURCE_CRYSTAL:
        return "RESOURCE_CRYSTAL";
    case ResourceType::RESOURCE_GEMS:
        return "RESOURCE_GEMS";
    case ResourceType::RESOURCE_ORE:
        return "RESOURCE_ORE";
    case ResourceType::RESOURCE_MERCURY:
        return "RESOURCE_MERCURY";
    case ResourceType::RESOURCE_GOLD:
        return "RESOURCE_GOLD";
    default:
        return NULL;
    }
}

inline ResourceType stringToResourceType(string resourceType) {
    if (resourceType == "RESOURCE_WOOD")
        return ResourceType::RESOURCE_WOOD;
    if (resourceType == "RESOURCE_SULFUR")
        return ResourceType::RESOURCE_SULFUR;
    if (resourceType == "RESOURCE_CRYSTAL")
        return ResourceType::RESOURCE_CRYSTAL;
    if (resourceType == "RESOURCE_GEMS")
        return ResourceType::RESOURCE_GEMS;
    if (resourceType == "RESOURCE_ORE")
        return ResourceType::RESOURCE_ORE;
    if (resourceType == "RESOURCE_MERCURY")
        return ResourceType::RESOURCE_MERCURY;
    if (resourceType == "RESOURCE_GOLD")
        return ResourceType::RESOURCE_GOLD;
    return ResourceType::RESOURCE_UNKNOWN;
}
