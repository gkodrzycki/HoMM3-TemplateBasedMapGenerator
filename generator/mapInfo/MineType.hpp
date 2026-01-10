#pragma once

#include "../global/Global.hpp"

enum class MineType {
    MINE_ALCHEMISTS_LAB = 0,
    MINE_CRYSTAL_CAVERN,
    MINE_GEM_POND,
    MINE_GOLD_MINE,
    MINE_ORE_PIT,
    MINE_SAWMILL,
    MINE_SULFUR_DUNE,
    MINE_UNKNOWN
};

string mineTypeToString(MineType mineType);

MineType stringToMineType(string mineType);

int3 getMineSize(MineType mineType);