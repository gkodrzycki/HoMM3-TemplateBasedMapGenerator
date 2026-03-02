#pragma once

#include "../global/Global.hpp"
#include "./ZoneLayout.hpp"

class ZoneLayout;

class RegionInfo {
  public:
    RegionInfo();

    int getID();
    string getName();
    vector<ZoneLayout> getZoneLayoutList();

    void deserializeRegion(const json &region);
    void printRegion();

  private:
    int id;
    string name;
    vector<ZoneLayout> zoneLayoutList;
};
