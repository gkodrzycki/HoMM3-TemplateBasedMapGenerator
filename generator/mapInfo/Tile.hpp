#pragma once

#include "../global/Global.hpp"
#include "./Road.hpp"

class Tile {
  public:
    Tile();
    Tile(int zoneID, string terrain);

    void setZoneID(int zoneID);
    void setTerrain(string terrain);
    void setRoad(shared_ptr<Road> road);

    int getZoneID();
    string getTerrain();
    shared_ptr<Road> getRoad();
    void printTile();

  private:
    int zoneID;
    string terrain;

    shared_ptr<Road> road;
};
