#pragma once

#include "../global/Global.hpp"
#include "../global/Random.hpp"
#include "../templateInfo/ZoneTemplate.hpp"
#include "./Faction.hpp"

enum class Faction;

class Zone {
  public:
    Zone(ZoneTemplate zoneTemplate, RNG &rng);

    void setCenter(int3 center);
    void setZoneID(int zoneID);
    void setSize(int size);
    void setTerrain(string terrain);
    void setFaction(Faction faction);
    void setOwner(int owner);
    void setType(string type);

    int3 getCenter();
    int getZoneID();
    int getSize();
    int getOwner();
    string getTerrain();
    Faction getFaction();
    string getType();

    void printZone();

  private:
    int3 center;

    int zoneID;
    int size;
    int owner;
    string terrain;
    Faction faction;
    string type;
};
