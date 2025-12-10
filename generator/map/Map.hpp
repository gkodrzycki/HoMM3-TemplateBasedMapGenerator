#pragma once

#include "../global/Global.hpp"
#include "../global/Random.hpp"
#include "../layoutInfo/LayoutInfo.hpp"
#include "../mapInfo/Tile.hpp"
#include "../mapInfo/Region.hpp"


class RNG;
class LayoutInfo;
class Tile;
class Region;


using RegionMap = map<int, shared_ptr<Region>>;
using TileMap = map<pair<int,int>, shared_ptr<Tile>>;


class Map
{
public:
    Map(RNG *rng);

    void generateMap(LayoutInfo layout);

    void printMap();
private:
    RNG *rng;

    RegionMap regionMap;
    TileMap tileMap;
};