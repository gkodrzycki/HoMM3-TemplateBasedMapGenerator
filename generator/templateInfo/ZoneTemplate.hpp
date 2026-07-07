#pragma once

#include "../global/Global.hpp"
#include "../mapInfo/MineType.hpp"

struct ZoneRestrictions {
    int minHumanPositions = 0;
    int maxHumanPositions = 0;
    int minTotalPositions = 0;
    int maxTotalPositions = 0;
};

struct TownSettings {
    int ownership         = 0;
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

struct AnchoredObjectRule {
    string objectName;
    string anchor = "castle"; // currently only "castle" is supported
    int distance  = 8;
    int count     = 1;
};

// Per-resource mine counts or density weights.
struct MineSettings {
    map<MineType, int> mineCounts;
};

struct ZoneMonsters {
    // "weak" / "normal" / "strong"
    string strength;
    // Faction names that can appear as guards, plus "Neutral".
    vector<string> factions;
    bool matchToTown = false;
    bool neutral     = false;
};

class ZoneTemplate {
  public:
    ZoneTemplate();

    int getId() const;
    // Zone type: "human_start", "computer_start", "treasure", "junction"
    string getType() const;
    int getBaseSize() const;

    const ZoneRestrictions &getRestrictions() const;
    const TownSettings &getPlayerTowns() const;
    const TownSettings &getNeutralTowns() const;
    const vector<string> &getTownTypes() const;
    const MineSettings &getMinimumMines() const;
    const MineSettings &getMineDensity() const;
    const vector<string> &getTerrain() const;
    const bool &getMatchTerrainToTown() const;
    const ZoneMonsters &getMonsters() const;
    const vector<TreasureTier> &getTreasure() const;
    const vector<AnchoredObjectRule> &getAnchoredObjects() const;

    // Zone-level options
    string getPlacement() const;
    string getObjects() const;
    int getMinimumObjects() const;
    string getImageSettings() const;

    bool getZoneRepulsion() const;
    int getMonstersDisposition() const;
    int getMonstersDispositionCustom() const;
    int getMonstersJoiningPercentage() const;
    bool getMonstersJoinOnlyForMoney() const;
    bool getForceNeutralCreatures() const;
    bool getAllowNonCoherentRoad() const;
    int getMaxBlockValue() const;
    int getMinimumAirshipShipyards() const;
    int getAirshipShipyardDensity() const;
    string getTownHint() const;
    string getTerrainHint() const;
    const vector<string> &getAllowedFactions() const;
    string getFactionHint() const;

    void deserialize(const json &zone);
    void print() const;

  private:
    int id;
    string type;
    int baseSize;

    ZoneRestrictions restrictions;
    TownSettings playerTowns;
    TownSettings neutralTowns;
    vector<string> townTypes;
    MineSettings minimumMines;
    MineSettings mineDensity;
    bool matchTerrainToTown;
    vector<string> terrain;
    ZoneMonsters monsters;
    vector<TreasureTier> treasure;
    vector<AnchoredObjectRule> anchoredObjects;

    // Options
    string placement;
    string objects;
    int minimumObjects;
    string imageSettings;
    bool forceNeutralCreatures;
    bool allowNonCoherentRoad;
    bool zoneRepulsion;
    string townHint;
    int monstersDisposition;
    int monstersDispositionCustom;
    int monstersJoiningPercentage;
    bool monstersJoinOnlyForMoney;
    int minimumAirshipShipyards;
    int airshipShipyardDensity;
    string terrainHint;
    vector<string> allowedFactions;
    string factionHint;
    int maxBlockValue;

    static MineSettings deserializeMines(const json &j);
    static TownSettings deserializeTownSettings(const json &j);
};
