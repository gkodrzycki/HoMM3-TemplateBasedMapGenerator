#include "./Zone.hpp"

string matchTerrainToTown(const string &townType) {
    if (townType == "Castle")
        return "GRASS";
    if (townType == "Rampart")
        return "GRASS";
    if (townType == "Tower")
        return "SNOW";
    if (townType == "Inferno")
        return "LAVA";
    if (townType == "Necropolis")
        return "DIRT";
    if (townType == "Dungeon")
        return "SUBTERRANEAN";
    if (townType == "Stronghold")
        return "ROUGH";
    if (townType == "Fortress")
        return "SWAMP";

    throw runtime_error("Unknown town type for terrain matching: " + townType);
}

Zone::Zone(ZoneTemplate zoneTemplate, RNG &rng) {
    vector<string> allowedFactions = zoneTemplate.getTownTypes();

    // Remove factions that are not playable in the base game (Cove, Factory, Bulwark)
    allowedFactions.erase(remove_if(allowedFactions.begin(), allowedFactions.end(),
                                    [](const string &f) {
                                        return f == "Cove" || f == "Factory" || f == "Bulwark" ||
                                               f == "Conflux";
                                    }),
                          allowedFactions.end());

    string chosenFaction =
        (allowedFactions.size() > 0) ? rng.getRandomFromVector(allowedFactions) : "NEUTRAL";
    this->setFaction(getEnumFromNameOrThrow<Faction>(chosenFaction));

    int owner = 0;
    if (zoneTemplate.getType() == "human_start" || zoneTemplate.getType() == "computer_start") {
        owner = zoneTemplate.getPlayerTowns().ownership;
    }

    this->setOwner(owner);
    this->setSize(zoneTemplate.getBaseSize());

    if (zoneTemplate.getMatchTerrainToTown()) {
        string terrain = matchTerrainToTown(chosenFaction);
        this->setTerrain(terrain);
    } else if (!zoneTemplate.getTerrain().empty()) {
        vector<string> terrainOptions = zoneTemplate.getTerrain();
        this->setTerrain(rng.getRandomFromVector(terrainOptions));
    } else {
        throw runtime_error("Zone template " + to_string(zoneTemplate.getId()) +
                            " has no terrain specified and does not match terrain to town");
    }

    this->setType(zoneTemplate.getType());
    this->setZoneID(zoneTemplate.getId());

    // this->setFaction(getEnumFromNameOrThrow<Faction>(zoneLayout.getFaction()));
    // this->setOwner(zoneLayout.getOwner());
    // this->setSize(decodeZoneSize(zoneLayout.getSize()));
    // this->setTerrain(zoneLayout.getTerrain());
    // this->setType(zoneLayout.getType());
    // this->setZoneID(zoneLayout.getID());
}

void Zone::setCenter(int3 center) { this->center = center; }

void Zone::setZoneID(int zoneID) { this->zoneID = zoneID; }

void Zone::setSize(int size) { this->size = size; }

void Zone::setTerrain(string terrain) { this->terrain = terrain; }

void Zone::setFaction(Faction faction) { this->faction = faction; }

void Zone::setOwner(int owner) { this->owner = owner; }

void Zone::setType(string type) { this->type = type; }

int3 Zone::getCenter() { return center; }

int Zone::getZoneID() { return zoneID; }

int Zone::getSize() { return size / 10; }

string Zone::getTerrain() { return terrain; }

Faction Zone::getFaction() { return faction; }

int Zone::getOwner() { return owner; }

string Zone::getType() { return type; }

void Zone::printZone() {
    cerr << "    Zone id: " << zoneID << "\n";
    cerr << "      Size: " << size << "\n";
    cerr << "      Terrain: " << terrain << "\n";
    cerr << "      Faction: " << getEnumName<Faction>(faction) << "\n";
    cerr << "      Owner: " << owner << "\n";
    cerr << "      Type: " << type << "\n";
    cerr << "      Center: " << center.toString() << "\n";
}
