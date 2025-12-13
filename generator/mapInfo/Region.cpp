#include "./Region.hpp"

Region::Region(RegionInfo regionInfo) {
    this->setRegionID(regionInfo.getID());
    this->setName(regionInfo.getName());
}

void Region::setRegionID(int regionID) { this->regionID = regionID; }

void Region::setName(string name) { this->name = name; }

int Region::getRegionID() { return regionID; }

string Region::getName() { return name; }

void Region::addZone(shared_ptr<Zone> zone) { zoneMap[zone->getZoneID()] = zone; }

shared_ptr<Zone> Region::getZone(int zoneID) {
    auto it = zoneMap.find(zoneID);
    if (it != zoneMap.end()) {
        return it->second;
    }
    return nullptr;
}

ZoneMap Region::getZoneMap() { return zoneMap; }

void Region::printRegion() {
    cerr << "  Region id: " << regionID << "\n";
    cerr << "  Region name: " << name << "\n";
    for (auto &zonePair : zoneMap) {
        zonePair.second->printZone();
    }
}
