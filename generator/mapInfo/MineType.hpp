#pragma once

#include "../global/Global.hpp"
#include "./ResourceType.hpp"

enum class MineType {
    MINE_SAWMILL = 0,
    MINE_ORE_PIT,

    MINE_ALCHEMISTS_LAB,
    MINE_CRYSTAL_CAVERN,
    MINE_GEM_POND,
    MINE_SULFUR_DUNE,
    MINE_GOLD_MINE,

    MINE_UNKNOWN
};

inline int3 getMineSize(MineType mineType) {
    switch (mineType) {
    case MineType::MINE_ALCHEMISTS_LAB:
        return int3(3, 1, 1);
    case MineType::MINE_CRYSTAL_CAVERN:
        return int3(3, 1, 1);
    case MineType::MINE_GEM_POND:
        return int3(3, 2, 1);
    case MineType::MINE_GOLD_MINE:
        return int3(3, 1, 1);
    case MineType::MINE_ORE_PIT:
        return int3(3, 2, 1);
    case MineType::MINE_SAWMILL:
        return int3(4, 2, 1);
    case MineType::MINE_SULFUR_DUNE:
        return int3(3, 1, 1);
    default:
        return int3(1, 1, 1);
    }
}

inline vector<string> getMineRealSize(MineType mineType) {
    switch (mineType) {
    case MineType::MINE_ALCHEMISTS_LAB:
        return vector<string>{"111"};
    case MineType::MINE_CRYSTAL_CAVERN:
        return vector<string>{"111"};
    case MineType::MINE_GEM_POND:
        return vector<string>{"111", "111"};
    case MineType::MINE_GOLD_MINE:
        return vector<string>{"111"};
    case MineType::MINE_ORE_PIT:
        return vector<string>{"111", "111"};
    case MineType::MINE_SAWMILL:
        return vector<string>{"0011", "1111"};
    case MineType::MINE_SULFUR_DUNE:
        return vector<string>{"111"};
    default:
        return vector<string>{"1"};
    }
}

inline ResourceType mineTypeToResourceType(MineType mineType) {
    switch (mineType) {
    case MineType::MINE_ALCHEMISTS_LAB:
        return ResourceType::RESOURCE_MERCURY;
    case MineType::MINE_CRYSTAL_CAVERN:
        return ResourceType::RESOURCE_CRYSTAL;
    case MineType::MINE_GEM_POND:
        return ResourceType::RESOURCE_GEMS;
    case MineType::MINE_GOLD_MINE:
        return ResourceType::RESOURCE_GOLD;
    case MineType::MINE_ORE_PIT:
        return ResourceType::RESOURCE_ORE;
    case MineType::MINE_SAWMILL:
        return ResourceType::RESOURCE_WOOD;
    case MineType::MINE_SULFUR_DUNE:
        return ResourceType::RESOURCE_SULFUR;
    default:
        return ResourceType::RESOURCE_UNKNOWN;
    }
}
