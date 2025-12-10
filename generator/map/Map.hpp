#pragma once

#include "../global/Global.hpp"
#include "../global/Random.hpp"
#include "../layoutInfo/LayoutInfo.hpp"
#include "../mapInfo/Region.hpp"
#include "../mapInfo/Tile.hpp"
#include "./placers/RegionPlacer.hpp"

class RNG;
class LayoutInfo;
class Tile;
class Region;

using RegionMap = map<int, shared_ptr<Region>>;
using TileMap = map<int, map<int, shared_ptr<Tile>>>;

class Map {
  public:
    Map(RNG &rng, LayoutInfo layoutInfo);

    void initTiles();

    void generateMap();

    void printMap();

    void addRegion(shared_ptr<Region> region);

    LayoutInfo getLayoutInfo();
    RNG &getRNG();

  private:
    RNG &rng;
    LayoutInfo layoutInfo;

    int width, height;

    RegionMap regionMap;
    TileMap tileMap;
};