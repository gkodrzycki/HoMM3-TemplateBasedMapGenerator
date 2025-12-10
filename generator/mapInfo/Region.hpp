#pragma once

#include "../global/Global.hpp"
#include "../layoutInfo/RegionInfo.hpp"
#include "./Zone.hpp"

class Zone;

using ZoneMap = map<int, shared_ptr<Zone>>;

class Region {
  public:
    Region(RegionInfo regionInfo);

    void setRegionID(int regionID);
    void setName(string name);

    int getRegionID();
    string getName();

    void addZone(shared_ptr<Zone> zone);
    shared_ptr<Zone> getZone(int zoneID);

    ZoneMap getZoneMap();

    void printRegion();

  private:
    int regionID;
    string name;

    ZoneMap zoneMap;
};