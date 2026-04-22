#include "./TemplateZone.hpp"

TemplateZone::TemplateZone()
    : id(0), role(""), baseSize(0), zoneRepulsion(false), monstersDisposition(0),
      monstersJoiningPercentage(0), monstersJoinOnlyForMoney(false), forceNeutralCreatures(false),
      allowNonCoherentRoad(false), minimumObjects(0), maxBlockValue(0) {}

int TemplateZone::getId() const { return id; }
string TemplateZone::getRole() const { return role; }
int TemplateZone::getBaseSize() const { return baseSize; }
const ZoneRestrictions &TemplateZone::getRestrictions() const { return restrictions; }
const TownSettings &TemplateZone::getPlayerTowns() const { return playerTowns; }
const TownSettings &TemplateZone::getNeutralTowns() const { return neutralTowns; }
const vector<string> &TemplateZone::getTownTypes() const { return townTypes; }
const MineSettings &TemplateZone::getMinimumMines() const { return minimumMines; }
const MineSettings &TemplateZone::getMineDensity() const { return mineDensity; }
const vector<string> &TemplateZone::getTerrain() const { return terrain; }
const ZoneMonsters &TemplateZone::getMonsters() const { return monsters; }
const vector<TreasureTier> &TemplateZone::getTreasure() const { return treasure; }
string TemplateZone::getObjects() const { return objects; }
string TemplateZone::getImageSettings() const { return imageSettings; }
bool TemplateZone::getZoneRepulsion() const { return zoneRepulsion; }
int TemplateZone::getMonstersDisposition() const { return monstersDisposition; }
int TemplateZone::getMonstersJoiningPercentage() const { return monstersJoiningPercentage; }
bool TemplateZone::getMonstersJoinOnlyForMoney() const { return monstersJoinOnlyForMoney; }
bool TemplateZone::getForceNeutralCreatures() const { return forceNeutralCreatures; }
bool TemplateZone::getAllowNonCoherentRoad() const { return allowNonCoherentRoad; }
int TemplateZone::getMinimumObjects() const { return minimumObjects; }
int TemplateZone::getMaxBlockValue() const { return maxBlockValue; }

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

MineSettings TemplateZone::deserializeMines(const json &j) {
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

TownSettings TemplateZone::deserializeTownSettings(const json &j) {
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

void TemplateZone::deserialize(const json &zone) {
    id = getOrError<int>(zone, "Id");

    // ---- Zone role (Type subsection) ------------------------------------
    if (zone.contains("Type")) {
        const auto &type = zone.at("Type");
        baseSize         = getOrDefault<int>(type, "Base Size", 0);
        if (getOrDefault<bool>(type, "human start", false))
            role = "human_start";
        else if (getOrDefault<bool>(type, "computer start", false))
            role = "computer_start";
        else if (getOrDefault<bool>(type, "Treasure", false))
            role = "treasure";
        else if (getOrDefault<bool>(type, "Junction", false))
            role = "junction";
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
        for (const auto &t : zone.at("Terrain"))
            terrain.push_back(t.get<string>());
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
            monsters.strength = getOrDefault<string>(m, "Strength", "");
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
        monstersJoiningPercentage = getOrDefault<int>(opts, "Monsters joining percentage", 0);
        monstersJoinOnlyForMoney  = getOrDefault<bool>(opts, "Monsters join only for money", false);
        forceNeutralCreatures     = getOrDefault<bool>(opts, "Force neutral creatures", false);
        allowNonCoherentRoad      = getOrDefault<bool>(opts, "Allow non-coherent road", false);
        minimumObjects            = getOrDefault<int>(opts, "Minimum objects", 0);
        maxBlockValue             = getOrDefault<int>(opts, "Max block value", 0);
    }
}

// ---------------------------------------------------------------------------
// Debug print
// ---------------------------------------------------------------------------

void TemplateZone::print() const {
    cerr << "    Zone id: " << id << "  role: " << role << "  baseSize: " << baseSize << "\n";
    cerr << "      Restrictions: human " << restrictions.minHumanPositions << "-"
         << restrictions.maxHumanPositions << "  total " << restrictions.minTotalPositions << "-"
         << restrictions.maxTotalPositions << "\n";
    if (playerTowns.ownership)
        cerr << "      Player towns: owner=" << playerTowns.ownership
             << "  castles>=" << playerTowns.minimumCastles << "\n";
    if (neutralTowns.minimumTowns || neutralTowns.minimumCastles)
        cerr << "      Neutral towns: towns>=" << neutralTowns.minimumTowns
             << "  castles>=" << neutralTowns.minimumCastles << "\n";
    if (!terrain.empty()) {
        cerr << "      Terrain:";
        for (const auto &t : terrain)
            cerr << " " << t;
        cerr << "\n";
    }
    if (!monsters.strength.empty())
        cerr << "      Monsters strength: " << monsters.strength << "\n";
    cerr << "      Treasure tiers: " << treasure.size() << "\n";
    for (const auto &t : treasure)
        cerr << "        " << t.low << "-" << t.high << " x" << t.density << "\n";
    if (!objects.empty())
        cerr << "      Objects: " << objects << "\n";
}
