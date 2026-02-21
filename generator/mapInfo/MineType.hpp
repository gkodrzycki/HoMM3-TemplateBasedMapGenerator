#pragma once

#include "../global/Global.hpp"

enum class MineType {
    MINE_ALCHEMISTS_LAB = 0,
    MINE_CRYSTAL_CAVERN,
    MINE_GEM_POND,
    MINE_SULFUR_DUNE,
    MINE_GOLD_MINE,

    MINE_ORE_PIT,
    MINE_SAWMILL,

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
