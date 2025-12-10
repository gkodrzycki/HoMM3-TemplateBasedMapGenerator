#pragma once

#include "../global/Global.hpp"
#include "./Zone.hpp"

class Zone;

using ZoneMap = map<int, shared_ptr<Zone>>;

class Region {
  public:
    Region();

    void setZoneID(int zoneID);
    void setTerrain(string terrain);

    int getZoneID();
    string getTerrain();

    void addZone(shared_ptr<Zone> zone);
    shared_ptr<Zone> getZone(int zoneID);

    ZoneMap getZoneMap();

  private:
    int regionID;
    string name;

    ZoneMap zoneMap;
};