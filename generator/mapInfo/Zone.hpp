#pragma once

#include "../global/Global.hpp"
#include "../layoutInfo/ZoneInfo.hpp"
#include "./Faction.hpp"

enum class Faction;

class Zone {
  public:
    Zone(ZoneInfo zoneInfo);

    void setCenter(int3 center);
    void setZoneID(int zoneID);
    void setSize(string size);
    void setTerrain(string terrain);
    void setFaction(Faction faction);
    void setOwner(string owner);
    void setType(string type);

    int3 getCenter();
    int getZoneID();
    string getSize();
    string getTerrain();
    Faction getFaction();
    string getOwner();
    string getType();

    void printZone();

  private:
    int3 center;

    int zoneID;
    string size;
    string terrain;
    Faction faction;
    string owner;
    string type;
};
