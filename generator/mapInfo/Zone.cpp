#include "./Zone.hpp"

Zone::Zone(ZoneInfo zoneInfo) {
    this->setFaction(getEnumFromNameOrThrow<Faction>(zoneInfo.getFaction()));
    this->setOwner(zoneInfo.getOwner());
    this->setSize(zoneInfo.getSize());
    this->setTerrain(zoneInfo.getTerrain());
    this->setType(zoneInfo.getType());
    this->setZoneID(zoneInfo.getID());
}

void Zone::setCenter(int3 center) { this->center = center; }

void Zone::setZoneID(int zoneID) { this->zoneID = zoneID; }

void Zone::setSize(string size) { this->size = size; }

void Zone::setTerrain(string terrain) { this->terrain = terrain; }

void Zone::setFaction(Faction faction) { this->faction = faction; }

void Zone::setOwner(string owner) { this->owner = owner; }

void Zone::setType(string type) { this->type = type; }

int3 Zone::getCenter() { return center; }

int Zone::getZoneID() { return zoneID; }

string Zone::getSize() { return size; }

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
