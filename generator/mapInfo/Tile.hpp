#pragma once

#include "../global/Global.hpp"
#include "./Road.hpp"

enum class TileType { TILE_FREE = 0, TILE_TAKEN };

class Tile {
  public:
    Tile();
    Tile(int zoneID, string terrain);
    Tile(int zoneID, string terrain, TileType tileType);

    void setZoneID(int zoneID);
    void setTerrain(string terrain);
    void setRoad(shared_ptr<Road> road);
    void setTileType(TileType tileType);

    int getZoneID();
    string getTerrain();
    shared_ptr<Road> getRoad();
    TileType getTileType();
    void printTile();

    string tileTypeToString(TileType tileType);
    TileType stringToTileType(string tileType);

  private:
    int zoneID;
    string terrain;
    TileType tileType;

    shared_ptr<Road> road;
};
