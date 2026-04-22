#include "./TemplateInfo.hpp"

TemplateInfo::TemplateInfo()
    : name(""), description(""), maxBattleRounds(0), mapName(""), minimumSize(0), maximumSize(0),
      mapObjects(""), specialWeeksDisabled(false), spellResearch(false) {}

string TemplateInfo::getName() const { return name; }
string TemplateInfo::getDescription() const { return description; }
int TemplateInfo::getMaxBattleRounds() const { return maxBattleRounds; }
string TemplateInfo::getMapName() const { return mapName; }
int TemplateInfo::getMinimumSize() const { return minimumSize; }
int TemplateInfo::getMaximumSize() const { return maximumSize; }
string TemplateInfo::getMapObjects() const { return mapObjects; }
bool TemplateInfo::getSpecialWeeksDisabled() const { return specialWeeksDisabled; }
bool TemplateInfo::getSpellResearch() const { return spellResearch; }

const vector<TemplateZone> &TemplateInfo::getZones() const { return zones; }
const vector<TemplateConnection> &TemplateInfo::getConnections() const { return connections; }

const TemplateZone &TemplateInfo::getZoneById(int id) const {
    for (const auto &z : zones) {
        if (z.getId() == id)
            return z;
    }
    throw runtime_error("TemplateZone with id " + to_string(id) + " not found.");
}

void TemplateInfo::deserialize(const json &tmpl) {
    // ---- Pack -----------------------------------------------------------
    if (tmpl.contains("pack")) {
        const auto &pack = tmpl.at("pack");
        if (pack.contains("Options")) {
            const auto &opts = pack.at("Options");
            name             = getOrDefault<string>(opts, "Name", "");
            description      = getOrDefault<string>(opts, "Description", "");
            maxBattleRounds  = getOrDefault<int>(opts, "Max Battle Rounds", 0);
        }
    }

    // ---- Map ------------------------------------------------------------
    if (tmpl.contains("map")) {
        const auto &map      = tmpl.at("map");
        mapName              = getOrDefault<string>(map, "Name", "");
        minimumSize          = getOrDefault<int>(map, "Minimum Size", 0);
        maximumSize          = getOrDefault<int>(map, "Maximum Size", 0);
        mapObjects           = getOrDefault<string>(map, "Objects", "");
        specialWeeksDisabled = getOrDefault<bool>(map, "Special weeks disabled", false);
        spellResearch        = getOrDefault<bool>(map, "Spell Research", false);
    }

    // ---- Zones ----------------------------------------------------------
    const auto &zoneList = getOrError<json>(tmpl, "zones");
    for (const auto &z : zoneList) {
        TemplateZone zone;
        zone.deserialize(z);
        zones.push_back(zone);
    }

    // ---- Connections ----------------------------------------------------
    const auto &connList = getOrError<json>(tmpl, "connections");
    for (const auto &c : connList) {
        TemplateConnection conn;
        conn.deserialize(c);
        connections.push_back(conn);
    }
}

void TemplateInfo::print() const {
    cerr << "Template: " << name << "\n";
    cerr << "Description: " << description << "\n";
    cerr << "Max Battle Rounds: " << maxBattleRounds << "\n";
    cerr << "Map: " << mapName << "  size " << minimumSize << "-" << maximumSize << "\n";
    if (specialWeeksDisabled)
        cerr << "Special weeks: disabled\n";
    if (spellResearch)
        cerr << "Spell Research: enabled\n";
    if (!mapObjects.empty())
        cerr << "Map objects: " << mapObjects << "\n";

    cerr << "Zones (" << zones.size() << "):\n";
    for (const auto &z : zones)
        z.print();

    cerr << "Connections (" << connections.size() << "):\n";
    for (const auto &c : connections)
        c.print();
}
