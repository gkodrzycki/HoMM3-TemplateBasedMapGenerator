#pragma once

#include "../global/Global.hpp"

class Tile {
  public:
    Tile();
    Tile(int zoneID, string terrain);

    void setZoneID(int zoneID);
    void setTerrain(string terrain);

    int getZoneID();
    string getTerrain();

    void printTile();

  private:
    int zoneID;
    string terrain;
};
