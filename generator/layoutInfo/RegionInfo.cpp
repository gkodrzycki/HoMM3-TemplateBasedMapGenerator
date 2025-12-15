#include "./RegionInfo.hpp"

RegionInfo::RegionInfo() : id(0), name("") {}

int RegionInfo::getID() { return id; }
string RegionInfo::getName() { return name; }
vector<ZoneInfo> RegionInfo::getZoneInfoList() { return zoneInfoList; }

void RegionInfo::deserializeRegion(const json &region) {
    int id = getOrError<int>(region, "id");
    string name = getOrError<string>(region, "name");

    this->id = id;
    this->name = name;

    RegionDefaults defaults;
    defaults.size = getOrDefault<string>(region, "size", "");
    defaults.terrain = getOrDefault<string>(region, "terrain", "");
    defaults.faction = getOrDefault<string>(region, "faction", "");
    defaults.owner = getOrDefault<string>(region, "owner", "");
    defaults.type = getOrDefault<string>(region, "type", "");

    const auto &zoneList = getOrError<json>(region, "zones");
    for (const auto &zone : zoneList) {
        ZoneInfo zoneInfo;
        zoneInfo.deserializeZone(zone, defaults);
        zoneInfoList.push_back(zoneInfo);
    }
}

void RegionInfo::printRegion() {
    cerr << "  Region id: " << id << "\n";
    cerr << "  Region name: " << name << "\n";

    for (auto zone : zoneInfoList) {
        zone.printZone();
    }
}
