#pragma once

#include "../global/Global.hpp"

struct RegionDefaults {
    string size = "";
    string terrain = "";
    string faction = "";
    string owner = "";
    string type = "";
};

class ZoneInfo {
  public:
    ZoneInfo();

    int getID();
    string getSize();
    string getTerrain();
    string getFaction();
    string getOwner();
    string getType();

    string getWithRegionFallback(const json &zone, const string &key, const string &regionDefault);
    void deserializeZone(const json &zone, const RegionDefaults &defaults = RegionDefaults());
    void printZone();

  private:
    int id;
    string size;
    string terrain;
    string faction;
    string owner;
    string type;
};
