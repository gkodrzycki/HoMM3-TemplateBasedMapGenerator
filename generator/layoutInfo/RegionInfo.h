#pragma once

#include "../global/Global.h"
#include "./ZoneInfo.h"

class ZoneInfo;

class RegionInfo {
public:

    RegionInfo();

    void deserializeRegion(const json& region);
    void printRegion();

private:
    int id;
    std::string name;
    std::vector<ZoneInfo> zoneInfoList;
};