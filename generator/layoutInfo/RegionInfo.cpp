#include "./RegionInfo.h"


RegionInfo::RegionInfo() : id(0), name("") {}

void RegionInfo::deserializeRegion(const json& region) {
    int id = getOrError<int>(region, "id");
    std::string name = getOrError<std::string>(region, "name");

    this->id = id; 
    this->name = name;

    const auto& zoneList = getOrError<json>(region, "zones");
    for (const auto& zone : zoneList) {
        ZoneInfo zoneInfo;
        zoneInfo.deserializeZone(zone);
        zoneInfoList.push_back(zoneInfo);
    }
}

void RegionInfo::printRegion() {
    std::cerr << "  Region id: " << id << "\n";
    std::cerr << "  Region name: " << name << "\n";

    for(auto zone : zoneInfoList){
        zone.printZone();
    }
}