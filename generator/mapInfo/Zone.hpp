#pragma once

#include "../global/Global.hpp"

class Zone {
  public:
    Zone();

    void setCenter(int3 center);
    void setZoneID(int zoneID);
    void setSize(string size);
    void setTerrain(string terrain);
    void setFaction(string faction);
    void setOwner(string owner);
    void setType(string type);

    int3 getCenter();
    int getZoneID();
    string getSize();
    string getTerrain();
    string getFaction();
    string getOwner();
    string getType();

  private:
    int3 center;

    int zoneID;
    string size;
    string terrain;
    string faction;
    string owner;
    string type;
};