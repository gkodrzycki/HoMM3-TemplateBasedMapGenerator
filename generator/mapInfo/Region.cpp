#include "./Region.hpp"

Region::Region() {}

void Region::setZoneID(int zoneID) { this->regionID = zoneID; }

void Region::setTerrain(string terrain) { this->name = terrain; }

int Region::getZoneID() { return regionID; }

string Region::getTerrain() { return name; }

void Region::addZone(shared_ptr<Zone> zone) { zoneMap[zone->getZoneID()] = zone; }

shared_ptr<Zone> Region::getZone(int zoneID) {
    auto it = zoneMap.find(zoneID);
    if (it != zoneMap.end()) {
        return it->second;
    }
    return nullptr;
}

ZoneMap Region::getZoneMap() { return zoneMap; }