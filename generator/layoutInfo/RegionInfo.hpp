#pragma once

#include "../global/Global.hpp"
#include "./ZoneInfo.hpp"

class ZoneInfo;

class RegionInfo {
  public:
    RegionInfo();

    int getID();
    string getName();
    vector<ZoneInfo> getZoneInfoList();

    void deserializeRegion(const json &region);
    void printRegion();

  private:
    int id;
    string name;
    vector<ZoneInfo> zoneInfoList;
};
