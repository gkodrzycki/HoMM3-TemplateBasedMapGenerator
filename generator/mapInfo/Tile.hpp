#pragma once

#include "../global/Global.hpp"
#include "./Road.hpp"

enum class TileType {
    TILE_FREE = 0,
    TILE_TAKEN,
    TILE_RESERVED,
    TILE_OCCUPIED,
    TILE_ROAD,
    TILE_BORDER_INNER,
    TILE_BORDER_OUTER
};

char tileTypeToChar(TileType tileType);
TileType charToTileType(char c);

class Tile {
  public:
    Tile();
    Tile(int zoneID, string terrain);
    Tile(int zoneID, string terrain, TileType tileType);

    void setZoneID(int zoneID);
    void setTerrain(string terrain);
    void setTileType(TileType tileType);

    int getZoneID();
    string getTerrain();
    TileType getTileType();
    void printTile();

    bool isTileType(string types);

  private:
    int zoneID;
    string terrain;
    TileType tileType;
};
