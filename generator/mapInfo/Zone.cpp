#include "./Zone.hpp"

int decodeZoneSize(string size) {
    if (size == "S")
        return 1;
    if (size == "M")
        return 2;
    if (size == "L")
        return 3;
    return 0;
}

Zone::Zone(ZoneLayout zoneLayout) {
    this->setFaction(getEnumFromNameOrThrow<Faction>(zoneLayout.getFaction()));
    this->setOwner(zoneLayout.getOwner());
    this->setSize(decodeZoneSize(zoneLayout.getSize()));
    this->setTerrain(zoneLayout.getTerrain());
    this->setType(zoneLayout.getType());
    this->setZoneID(zoneLayout.getID());
}

void Zone::setCenter(int3 center) { this->center = center; }

void Zone::setZoneID(int zoneID) { this->zoneID = zoneID; }

void Zone::setSize(int size) { this->size = size; }

void Zone::setTerrain(string terrain) { this->terrain = terrain; }

void Zone::setFaction(Faction faction) { this->faction = faction; }

void Zone::setOwner(string owner) { this->owner = owner; }

void Zone::setType(string type) { this->type = type; }

int3 Zone::getCenter() { return center; }

int Zone::getZoneID() { return zoneID; }

int Zone::getSize() { return size; }

string Zone::getTerrain() { return terrain; }

Faction Zone::getFaction() { return faction; }

string Zone::getOwner() { return owner; }

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
