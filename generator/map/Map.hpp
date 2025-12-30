#pragma once

#include "../global/Global.hpp"
#include "../global/Random.hpp"
#include "../layoutInfo/LayoutInfo.hpp"
#include "../mapInfo/Object.hpp"
#include "../mapInfo/Region.hpp"
#include "../mapInfo/Tile.hpp"
#include "../mapInfo/Zone.hpp"
#include "./placers/ObjectPlacer.hpp"
#include "./placers/RegionPlacer.hpp"

class RNG;
class LayoutInfo;
class Tile;
class Region;
class Zone;
class Object;

using ZoneMap      = map<int, shared_ptr<Zone>>;
using RegionMap    = map<int, shared_ptr<Region>>;
using TileMap      = map<int, map<int, shared_ptr<Tile>>>;
using ObjectVector = vector<shared_ptr<Object>>;

class Map {
  public:
    Map(RNG &rng, LayoutInfo layoutInfo);

    void initTiles();

    void generateMap();

    void printMap();

    void addRegion(shared_ptr<Region> region);
    void addZone(shared_ptr<Zone> zone);
    void addObject(shared_ptr<Object> object);

    shared_ptr<Tile> getTile(int3 pos);
    LayoutInfo getLayoutInfo();
    RegionMap getRegionMap();
    ZoneMap getZoneMap();
    const TileMap &getTileMap();
    ObjectVector getObjectVector();
    RNG &getRNG();
    int getWidth();
    int getHeight();

  private:
    RNG &rng;
    LayoutInfo layoutInfo;

    int width, height;

    RegionMap regionMap;
    ZoneMap zoneMap;
    TileMap tileMap;
    ObjectVector objectVector;
};
