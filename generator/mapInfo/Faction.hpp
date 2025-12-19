#pragma once

#include "../global/Global.hpp"

enum class Faction {
    CASTLE,
    RAMPART,
    TOWER,
    INFERNO,
    NECROPOLIS,
    DUNGEON,
    STRONGHOLD,
    FORTRESS,
    CONFLUX,
    NEUTRAL,
    NONE,
    DEFAULT
};

inline string factionToString(Faction faction) {
    switch (faction) {
    case Faction::CASTLE:
        return "CASTLE";
    case Faction::RAMPART:
        return "RAMPART";
    case Faction::TOWER:
        return "TOWER";
    case Faction::INFERNO:
        return "INFERNO";
    case Faction::NECROPOLIS:
        return "NECROPOLIS";
    case Faction::DUNGEON:
        return "DUNGEON";
    case Faction::STRONGHOLD:
        return "STRONGHOLD";
    case Faction::FORTRESS:
        return "FORTRESS";
    case Faction::CONFLUX:
        return "CONFLUX";
    case Faction::NEUTRAL:
        return "NEUTRAL";
    case Faction::NONE:
        return "NONE";
    case Faction::DEFAULT:
        return "DEFAULT";
    }
    return "DEFAULT";
}

inline Faction stringToFaction(string faction) {
    if (faction == "Castle") {
        return Faction::CASTLE;
    } else if (faction == "Rampart") {
        return Faction::RAMPART;
    } else if (faction == "Tower") {
        return Faction::TOWER;
    } else if (faction == "Inferno") {
        return Faction::INFERNO;
    } else if (faction == "Necropolis") {
        return Faction::NECROPOLIS;
    } else if (faction == "Dungeon") {
        return Faction::DUNGEON;
    } else if (faction == "Stronghold") {
        return Faction::STRONGHOLD;
    } else if (faction == "Fortress") {
        return Faction::FORTRESS;
    } else if (faction == "Conflux") {
        return Faction::CONFLUX;
    } else if (faction == "Neutral") {
        return Faction::NEUTRAL;
    } else if (faction == "None") {
        return Faction::NONE;
    }
    return Faction::DEFAULT;
}
