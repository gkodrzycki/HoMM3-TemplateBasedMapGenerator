#pragma once

#include "../global/Global.hpp"

struct ZoneRestrictions {
    int minHumanPositions = 0;
    int maxHumanPositions = 0;
    int minTotalPositions = 0;
    int maxTotalPositions = 0;
};

struct TownSettings {
    int ownership         = 0; // 1-based player index; 0 = neutral
    int minimumTowns      = 0;
    int minimumCastles    = 0;
    int townDensity       = 0;
    int castleDensity     = 0;
    bool townsAreSameType = false;
};

// One treasure tier: a gold-value range and a density weight.
struct TreasureTier {
    int low     = 0;
    int high    = 0;
    int density = 0;
};

// Per-resource mine counts or density weights.
struct MineSettings {
    int wood    = 0;
    int mercury = 0;
    int ore     = 0;
    int sulfur  = 0;
    int crystal = 0;
    int gems    = 0;
    int gold    = 0;
};

struct ZoneMonsters {
    // "weak" / "normal" / "strong"
    string strength;
    // Faction names that can appear as guards, plus "Neutral".
    vector<string> factions;
};

class TemplateZone {
  public:
    TemplateZone();

    int getId() const;
    // Zone role: "human_start", "computer_start", "treasure", "junction"
    string getRole() const;
    int getBaseSize() const;

    const ZoneRestrictions &getRestrictions() const;
    const TownSettings &getPlayerTowns() const;
    const TownSettings &getNeutralTowns() const;
    const vector<string> &getTownTypes() const;
    const MineSettings &getMinimumMines() const;
    const MineSettings &getMineDensity() const;
    const vector<string> &getTerrain() const;
    const ZoneMonsters &getMonsters() const;
    const vector<TreasureTier> &getTreasure() const;

    // Zone-level options
    string getObjects() const;
    string getImageSettings() const;
    bool getZoneRepulsion() const;
    int getMonstersDisposition() const;
    int getMonstersJoiningPercentage() const;
    bool getMonstersJoinOnlyForMoney() const;
    bool getForceNeutralCreatures() const;
    bool getAllowNonCoherentRoad() const;
    int getMinimumObjects() const;
    int getMaxBlockValue() const;

    void deserialize(const json &zone);
    void print() const;

  private:
    int id;
    string role;
    int baseSize;

    ZoneRestrictions restrictions;
    TownSettings playerTowns;
    TownSettings neutralTowns;
    vector<string> townTypes;
    MineSettings minimumMines;
    MineSettings mineDensity;
    vector<string> terrain;
    ZoneMonsters monsters;
    vector<TreasureTier> treasure;

    // Options
    string objects;
    string imageSettings;
    bool zoneRepulsion;
    int monstersDisposition;
    int monstersJoiningPercentage;
    bool monstersJoinOnlyForMoney;
    bool forceNeutralCreatures;
    bool allowNonCoherentRoad;
    int minimumObjects;
    int maxBlockValue;

    static MineSettings deserializeMines(const json &j);
    static TownSettings deserializeTownSettings(const json &j);
};
