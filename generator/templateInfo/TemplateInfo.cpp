#include "./TemplateInfo.hpp"

TemplateInfo::TemplateInfo()
    : name(""), description(""), townSelection(""), heroes(""), mirror(false), tags(""),
      maxBattleRounds(0), forbidHiringHeroes(false), mapName(""), minimumSize(0), maximumSize(0),
      artifacts(""), comboArts(""), spells(""), secondarySkills(""), mapObjects(""), rockBlocks(""),
      zoneSparseness(0), specialWeeksDisabled(false), spellResearch(false), anarchy(false) {}

string TemplateInfo::getName() const { return name; }
string TemplateInfo::getDescription() const { return description; }
string TemplateInfo::getTownSelection() const { return townSelection; }
string TemplateInfo::getHeroes() const { return heroes; }
bool TemplateInfo::getMirror() const { return mirror; }
string TemplateInfo::getTags() const { return tags; }
int TemplateInfo::getMaxBattleRounds() const { return maxBattleRounds; }
bool TemplateInfo::getForbidHiringHeroes() const { return forbidHiringHeroes; }
string TemplateInfo::getMapName() const { return mapName; }
int TemplateInfo::getMinimumSize() const { return minimumSize; }
int TemplateInfo::getMaximumSize() const { return maximumSize; }
string TemplateInfo::getArtifacts() const { return artifacts; }
string TemplateInfo::getComboArts() const { return comboArts; }
string TemplateInfo::getSpells() const { return spells; }
string TemplateInfo::getSecondarySkills() const { return secondarySkills; }
string TemplateInfo::getMapObjects() const { return mapObjects; }
string TemplateInfo::getRockBlocks() const { return rockBlocks; }
int TemplateInfo::getZoneSparseness() const { return zoneSparseness; }
bool TemplateInfo::getSpecialWeeksDisabled() const { return specialWeeksDisabled; }
bool TemplateInfo::getSpellResearch() const { return spellResearch; }
bool TemplateInfo::getAnarchy() const { return anarchy; }

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
            const auto &opts   = pack.at("Options");
            name               = getOrDefault<string>(opts, "Name", "");
            description        = getOrDefault<string>(opts, "Description", "");
            townSelection      = getOrDefault<string>(opts, "Town selection", "");
            heroes             = getOrDefault<string>(opts, "Heroes", "");
            mirror             = getOrDefault<bool>(opts, "Mirror", false);
            tags               = getOrDefault<string>(opts, "Tags", "");
            maxBattleRounds    = getOrDefault<int>(opts, "Max Battle Rounds", 0);
            forbidHiringHeroes = getOrDefault<bool>(opts, "Forbid Hiring Heroes", false);
        }
    }

    // ---- Map ------------------------------------------------------------
    if (tmpl.contains("map")) {
        const auto &map      = tmpl.at("map");
        mapName              = getOrDefault<string>(map, "Name", "");
        minimumSize          = getOrDefault<int>(map, "Minimum Size", 0);
        maximumSize          = getOrDefault<int>(map, "Maximum Size", 0);
        artifacts            = getOrDefault<string>(map, "Artifacts", "");
        comboArts            = getOrDefault<string>(map, "Combo Arts", "");
        spells               = getOrDefault<string>(map, "Spells", "");
        secondarySkills      = getOrDefault<string>(map, "Secondary skills", "");
        mapObjects           = getOrDefault<string>(map, "Objects", "");
        rockBlocks           = getOrDefault<string>(map, "Rock blocks", "");
        zoneSparseness       = getOrDefault<int>(map, "Zone sparseness", 0);
        specialWeeksDisabled = getOrDefault<bool>(map, "Special weeks disabled", false);
        spellResearch        = getOrDefault<bool>(map, "Spell Research", false);
        anarchy              = getOrDefault<bool>(map, "Anarchy", false);
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
    if (!townSelection.empty())
        cerr << "Town selection: " << townSelection << "\n";
    if (!heroes.empty())
        cerr << "Heroes: " << heroes << "\n";
    if (mirror)
        cerr << "Mirror: true\n";
    if (!tags.empty())
        cerr << "Tags: " << tags << "\n";
    if (forbidHiringHeroes)
        cerr << "Forbid Hiring Heroes: true\n";
    cerr << "Map: " << mapName << "  size " << minimumSize << "-" << maximumSize << "\n";
    if (specialWeeksDisabled)
        cerr << "Special weeks: disabled\n";
    if (spellResearch)
        cerr << "Spell Research: enabled\n";
    if (!mapObjects.empty())
        cerr << "Map objects: " << mapObjects << "\n";
    if (!artifacts.empty())
        cerr << "Map artifacts: " << artifacts << "\n";
    if (!comboArts.empty())
        cerr << "Map combo arts: " << comboArts << "\n";
    if (!spells.empty())
        cerr << "Map spells: " << spells << "\n";
    if (!secondarySkills.empty())
        cerr << "Map secondary skills: " << secondarySkills << "\n";
    if (!rockBlocks.empty())
        cerr << "Rock blocks: " << rockBlocks << "\n";
    if (zoneSparseness)
        cerr << "Zone sparseness: " << zoneSparseness << "\n";
    if (anarchy)
        cerr << "Anarchy: true\n";

    cerr << "Zones (" << zones.size() << "):\n";
    for (const auto &z : zones)
        z.print();

    cerr << "Connections (" << connections.size() << "):\n";
    for (const auto &c : connections)
        c.print();
}
