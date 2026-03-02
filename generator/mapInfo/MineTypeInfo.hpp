#pragma once

#include "../global/Global.hpp"
#include "../global/Random.hpp"
#include "./Faction.hpp"

enum class MineTypeInfo {
    ALCHEMISTS_LAB = 0,
    CRYSTAL_CAVERN,
    GEM_POND,
    SULFUR_DUNE,
    GOLD_MINE,

    ORE_PIT,
    SAWMILL,

    FACTION_MINE,
    RANDOM_MINE,

    UNKNOWN
};

inline string getMineType(MineTypeInfo mineTypeInfo, RNG &rng, Faction faction) {
    switch (mineTypeInfo) {
    case MineTypeInfo::FACTION_MINE: {
        switch (faction) {
        case Faction::CASTLE:
            return "GEM POND";
        case Faction::RAMPART:
            return "CRYSTAL CAVERN";
        case Faction::TOWER:
            return "GEM POND";
        case Faction::INFERNO:
            return "AlCHEMISTS LAB";
        case Faction::NECROPOLIS:
            return "AlCHEMISTS LAB";
        case Faction::DUNGEON:
            return "SULFUR DUNE";
        case Faction::STRONGHOLD:
            return "CRYSTAL CAVERN";
        case Faction::FORTRESS:
            return "SULFUR DUNE";
        case Faction::CONFLUX:
            return "ALCHEMISTS LAB";
        default:
            return "GOLD MINE";
        }
    }
    default:
        return getEnumName<MineTypeInfo>(mineTypeInfo);
    }
}
