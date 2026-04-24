#include "./ZoneTemplate.hpp"

ZoneTemplate::ZoneTemplate()
    : id(0), type(""), baseSize(0), restrictions(), playerTowns(), neutralTowns(), townTypes(),
      minimumMines(), mineDensity(), terrain(), monsters(), treasure(), placement(""), objects(""),
      minimumObjects(0), imageSettings(""), forceNeutralCreatures(false),
      allowNonCoherentRoad(false), zoneRepulsion(false), townHint(""), monstersDisposition(0),
      monstersDispositionCustom(0), monstersJoiningPercentage(0), monstersJoinOnlyForMoney(false),
      minimumAirshipShipyards(0), airshipShipyardDensity(0), terrainHint(""), allowedFactions(),
      factionHint(""), maxBlockValue(0) {}

int ZoneTemplate::getId() const { return id; }
string ZoneTemplate::getType() const { return type; }
int ZoneTemplate::getBaseSize() const { return baseSize; }
const ZoneRestrictions &ZoneTemplate::getRestrictions() const { return restrictions; }
const TownSettings &ZoneTemplate::getPlayerTowns() const { return playerTowns; }
const TownSettings &ZoneTemplate::getNeutralTowns() const { return neutralTowns; }
const vector<string> &ZoneTemplate::getTownTypes() const { return townTypes; }
const MineSettings &ZoneTemplate::getMinimumMines() const { return minimumMines; }
const MineSettings &ZoneTemplate::getMineDensity() const { return mineDensity; }
const vector<string> &ZoneTemplate::getTerrain() const { return terrain; }
const bool &ZoneTemplate::getMatchTerrainToTown() const { return matchTerrainToTown; }

const ZoneMonsters &ZoneTemplate::getMonsters() const { return monsters; }
const vector<TreasureTier> &ZoneTemplate::getTreasure() const { return treasure; }
string ZoneTemplate::getObjects() const { return objects; }
string ZoneTemplate::getImageSettings() const { return imageSettings; }
bool ZoneTemplate::getZoneRepulsion() const { return zoneRepulsion; }
int ZoneTemplate::getMonstersDisposition() const { return monstersDisposition; }
int ZoneTemplate::getMonstersDispositionCustom() const { return monstersDispositionCustom; }
int ZoneTemplate::getMonstersJoiningPercentage() const { return monstersJoiningPercentage; }
bool ZoneTemplate::getMonstersJoinOnlyForMoney() const { return monstersJoinOnlyForMoney; }
bool ZoneTemplate::getForceNeutralCreatures() const { return forceNeutralCreatures; }
bool ZoneTemplate::getAllowNonCoherentRoad() const { return allowNonCoherentRoad; }
int ZoneTemplate::getMinimumObjects() const { return minimumObjects; }
int ZoneTemplate::getMaxBlockValue() const { return maxBlockValue; }
string ZoneTemplate::getPlacement() const { return placement; }
int ZoneTemplate::getMinimumAirshipShipyards() const { return minimumAirshipShipyards; }
int ZoneTemplate::getAirshipShipyardDensity() const { return airshipShipyardDensity; }
string ZoneTemplate::getTownHint() const { return townHint; }
string ZoneTemplate::getTerrainHint() const { return terrainHint; }
const vector<string> &ZoneTemplate::getAllowedFactions() const { return allowedFactions; }
string ZoneTemplate::getFactionHint() const { return factionHint; }

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

MineSettings ZoneTemplate::deserializeMines(const json &j) {
    MineSettings m;
    m.wood    = getOrDefault<int>(j, "Wood", 0);
    m.mercury = getOrDefault<int>(j, "Mercury", 0);
    m.ore     = getOrDefault<int>(j, "Ore", 0);
    m.sulfur  = getOrDefault<int>(j, "Sulfur", 0);
    m.crystal = getOrDefault<int>(j, "Crystal", 0);
    m.gems    = getOrDefault<int>(j, "Gems", 0);
    m.gold    = getOrDefault<int>(j, "Gold", 0);
    return m;
}

TownSettings ZoneTemplate::deserializeTownSettings(const json &j) {
    TownSettings t;
    t.ownership        = getOrDefault<int>(j, "Ownership", 0);
    t.minimumTowns     = getOrDefault<int>(j, "Minimum towns", 0);
    t.minimumCastles   = getOrDefault<int>(j, "Minimum castles", 0);
    t.townDensity      = getOrDefault<int>(j, "Town Density", 0);
    t.castleDensity    = getOrDefault<int>(j, "Castle Density", 0);
    t.townsAreSameType = getOrDefault<bool>(j, "Towns are of same type", false);
    return t;
}

// ---------------------------------------------------------------------------
// Deserialization
// ---------------------------------------------------------------------------

void ZoneTemplate::deserialize(const json &zone) {
    id = getOrError<int>(zone, "Id");

    // ---- Zone role (Type subsection) ------------------------------------
    if (zone.contains("Type")) {
        const auto &_type = zone.at("Type");
        baseSize          = getOrDefault<int>(_type, "Base Size", 0);
        if (getOrDefault<bool>(_type, "human start", false))
            type = "human_start";
        else if (getOrDefault<bool>(_type, "computer start", false))
            type = "computer_start";
        else if (getOrDefault<bool>(_type, "Treasure", false))
            type = "treasure";
        else if (getOrDefault<bool>(_type, "Junction", false))
            type = "junction";
    }

    // ---- Restrictions ---------------------------------------------------
    if (zone.contains("Restrictions")) {
        const auto &r                  = zone.at("Restrictions");
        restrictions.minHumanPositions = getOrDefault<int>(r, "Minimum human positions", 0);
        restrictions.maxHumanPositions = getOrDefault<int>(r, "Maximum human positions", 0);
        restrictions.minTotalPositions = getOrDefault<int>(r, "Minimum total positions", 0);
        restrictions.maxTotalPositions = getOrDefault<int>(r, "Maximum total positions", 0);
    }

    // ---- Towns ----------------------------------------------------------
    if (zone.contains("Player towns"))
        playerTowns = deserializeTownSettings(zone.at("Player towns"));
    if (zone.contains("Neutral towns"))
        neutralTowns = deserializeTownSettings(zone.at("Neutral towns"));

    if (zone.contains("Town types")) {
        for (const auto &f : zone.at("Town types"))
            townTypes.push_back(f.get<string>());
    }

    // ---- Mines ----------------------------------------------------------
    if (zone.contains("Minimum mines"))
        minimumMines = deserializeMines(zone.at("Minimum mines"));
    if (zone.contains("Mine Density"))
        mineDensity = deserializeMines(zone.at("Mine Density"));

    // ---- Terrain --------------------------------------------------------
    if (zone.contains("Terrain")) {
        if (zone.at("Terrain")[0] == "Match to town") {
            matchTerrainToTown = true;
        } else {
            for (const auto &t : zone.at("Terrain")) {
                terrain.push_back(t.get<string>());
            }
        }
    }

    // ---- Monsters -------------------------------------------------------
    // Monsters is either a plain array (only factions, no strength) or an
    // object { "Strength": "...", "list": [...] }.
    if (zone.contains("Monsters")) {
        const auto &m = zone.at("Monsters");
        if (m.is_array()) {
            for (const auto &f : m)
                monsters.factions.push_back(f.get<string>());
        } else if (m.is_object()) {
            monsters.strength    = getOrDefault<string>(m, "Strength", "");
            monsters.matchToTown = getOrDefault<bool>(m, "Match to town", false);
            monsters.neutral     = getOrDefault<bool>(m, "Neutral", false);
            if (m.contains("list")) {
                for (const auto &f : m.at("list"))
                    monsters.factions.push_back(f.get<string>());
            }
        }
    }

    // ---- Treasure tiers -------------------------------------------------
    if (zone.contains("Treasure")) {
        const auto &tList = zone.at("Treasure");
        // Can be a single object or an array of tiers.
        if (tList.is_array()) {
            for (const auto &tier : tList) {
                TreasureTier t;
                t.low     = getOrDefault<int>(tier, "Low", 0);
                t.high    = getOrDefault<int>(tier, "High", 0);
                t.density = getOrDefault<int>(tier, "Density", 0);
                treasure.push_back(t);
            }
        } else if (tList.is_object()) {
            TreasureTier t;
            t.low     = getOrDefault<int>(tList, "Low", 0);
            t.high    = getOrDefault<int>(tList, "High", 0);
            t.density = getOrDefault<int>(tList, "Density", 0);
            treasure.push_back(t);
        }
    }

    // ---- Per-zone options -----------------------------------------------
    if (zone.contains("Options")) {
        const auto &opts          = zone.at("Options");
        objects                   = getOrDefault<string>(opts, "Objects", "");
        imageSettings             = getOrDefault<string>(opts, "Image settings", "");
        zoneRepulsion             = getOrDefault<bool>(opts, "Zone repulsion", false);
        monstersDisposition       = getOrDefault<int>(opts, "Monsters disposition (standard)", 0);
        monstersDispositionCustom = getOrDefault<int>(opts, "Monsters disposition (custom)", 0);
        monstersJoiningPercentage = getOrDefault<int>(opts, "Monsters joining percentage", 0);
        monstersJoinOnlyForMoney  = getOrDefault<bool>(opts, "Monsters join only for money", false);
        forceNeutralCreatures     = getOrDefault<bool>(opts, "Force neutral creatures", false);
        allowNonCoherentRoad      = getOrDefault<bool>(opts, "Allow non-coherent road", false);
        minimumObjects            = getOrDefault<int>(opts, "Minimum objects", 0);
        maxBlockValue             = getOrDefault<int>(opts, "Max block value", 0);
        placement                 = getOrDefault<string>(opts, "Placement", "");
        minimumAirshipShipyards   = getOrDefault<int>(opts, "Minimum airship shipyards", 0);
        airshipShipyardDensity    = getOrDefault<int>(opts, "Airship shipyard Density", 0);
        townHint                  = getOrDefault<string>(opts, "Town Hint", "");
        terrainHint               = getOrDefault<string>(opts, "Terrain Hint", "");
        factionHint               = getOrDefault<string>(opts, "Faction Hint", "");
        if (opts.contains("Allowed Factions")) {
            for (const auto &f : opts.at("Allowed Factions"))
                allowedFactions.push_back(f.get<string>());
        }
    }
}

// ---------------------------------------------------------------------------
// Debug print
// ---------------------------------------------------------------------------

void ZoneTemplate::print() const {
    cerr << "    Zone id: " << id << "  type: " << type << "  baseSize: " << baseSize << "\n";
    cerr << "      Restrictions: human " << restrictions.minHumanPositions << "-"
         << restrictions.maxHumanPositions << "  total " << restrictions.minTotalPositions << "-"
         << restrictions.maxTotalPositions << "\n";
    if (playerTowns.ownership)
        cerr << "      Player towns: owner=" << playerTowns.ownership
             << "  castles>=" << playerTowns.minimumCastles << "\n";
    if (neutralTowns.minimumTowns || neutralTowns.minimumCastles)
        cerr << "      Neutral towns: towns>=" << neutralTowns.minimumTowns
             << "  castles>=" << neutralTowns.minimumCastles << "\n";

    if (!townTypes.empty()) {
        cerr << "      Town types:\n";
        for (const auto &t : townTypes)
            cerr << "       " << t << "\n";
    }

    // Print only non-zero mine settings using shared helper.
    printNonZeroFields("Minimum mines", {{"Wood", minimumMines.wood},
                                         {"Mercury", minimumMines.mercury},
                                         {"Ore", minimumMines.ore},
                                         {"Sulfur", minimumMines.sulfur},
                                         {"Crystal", minimumMines.crystal},
                                         {"Gems", minimumMines.gems},
                                         {"Gold", minimumMines.gold}});

    printNonZeroFields("Mine density", {{"Wood", mineDensity.wood},
                                        {"Mercury", mineDensity.mercury},
                                        {"Ore", mineDensity.ore},
                                        {"Sulfur", mineDensity.sulfur},
                                        {"Crystal", mineDensity.crystal},
                                        {"Gems", mineDensity.gems},
                                        {"Gold", mineDensity.gold}});

    if (!terrain.empty()) {
        cerr << "      Terrain:";
        for (const auto &t : terrain)
            cerr << " " << t;
        cerr << "\n";
    }
    if (!monsters.strength.empty())
        cerr << "      Monsters strength: " << monsters.strength << "\n";
    if (!monsters.factions.empty()) {
        cerr << "      Monster factions:\n";
        for (const auto &f : monsters.factions)
            cerr << "       " << f << "\n";
    }
    if (monsters.matchToTown)
        cerr << "      Monsters: match to town\n";
    if (monsters.neutral)
        cerr << "      Monsters: include Neutral\n";
    cerr << "      Treasure tiers: " << treasure.size() << "\n";
    for (const auto &t : treasure)
        cerr << "        " << t.low << "-" << t.high << " density " << t.density << "\n";
    if (!objects.empty())
        cerr << "      Objects: " << objects << "\n";
    if (!placement.empty())
        cerr << "      Placement: " << placement << "\n";
    if (!allowedFactions.empty()) {
        cerr << "      Allowed Factions:";
        for (const auto &f : allowedFactions)
            cerr << " " << f;
        cerr << "\n";
    }
    if (!townHint.empty())
        cerr << "      Town Hint: " << townHint << "\n";
    if (!terrainHint.empty())
        cerr << "      Terrain Hint: " << terrainHint << "\n";
    if (monstersDispositionCustom)
        cerr << "      Monsters disposition (custom): " << monstersDispositionCustom << "\n";
    if (minimumAirshipShipyards)
        cerr << "      Minimum airship shipyards: " << minimumAirshipShipyards << "\n";
    if (airshipShipyardDensity)
        cerr << "      Airship shipyard Density: " << airshipShipyardDensity << "\n";
}
