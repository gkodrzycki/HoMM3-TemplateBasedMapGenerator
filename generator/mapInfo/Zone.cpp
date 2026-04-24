#include "./Zone.hpp"

Zone::Zone(ZoneTemplate zoneTemplate) {
    vector<string> allowedFactions = zoneTemplate.getTownTypes();
    // TODO add rng and select random faction from allowed factions
    this->setFaction(getEnumFromNameOrThrow<Faction>(allowedFactions[0]));

    int owner = 0;
    if (zoneTemplate.getType() == "human_start" || zoneTemplate.getType() == "computer_start") {
        owner = zoneTemplate.getPlayerTowns().ownership;
    }

    this->setOwner(owner);
    this->setSize(zoneTemplate.getBaseSize());

    if (zoneTemplate.getMatchTerrainToTown()) {
        cerr << "Zone template " << zoneTemplate.getId()
             << " matches terrain to town. Setting terrain to GRASS.\n";
        this->setTerrain("Grass");
    } else if (!zoneTemplate.getTerrain().empty()) {
        // TODO add rng and choose random terrain from zoneTemplate.getTerrain()
        cerr << "Zone template " << zoneTemplate.getId()
             << " has multiple terrains specified. Choosing the first one: "
             << zoneTemplate.getTerrain()[0] << "\n";
        this->setTerrain(zoneTemplate.getTerrain()[0]);
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
