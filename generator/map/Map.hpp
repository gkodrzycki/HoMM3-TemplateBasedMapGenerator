#pragma once

#include "../blueprintInfo/BlueprintInfo.hpp"
#include "../global/CellularAutomata.hpp"
#include "../global/Global.hpp"
#include "../global/Random.hpp"
#include "../layoutInfo/LayoutInfo.hpp"
#include "../mapInfo/Creature.hpp"
#include "../mapInfo/Object.hpp"
#include "../mapInfo/Region.hpp"
#include "../mapInfo/Resource.hpp"
#include "../mapInfo/Tile.hpp"
#include "../mapInfo/Zone.hpp"
#include "./placers/BorderPlacer.hpp"
#include "./placers/GuardPlacer.hpp"
#include "./placers/ObjectPlacer.hpp"
#include "./placers/RegionPlacer.hpp"
#include "./placers/RoadPlacer.hpp"
#include "./placers/TerrainPlacer.hpp"
#include "./placers/TownPlacer.hpp"

using ZoneMap        = map<int, shared_ptr<Zone>>;
using RegionMap      = map<int, shared_ptr<Region>>;
using TileMap        = map<int, map<int, shared_ptr<Tile>>>;
using ObjectVector   = vector<shared_ptr<Object>>;
using CreatureVector = vector<shared_ptr<Creature>>;
using RoadVector     = vector<shared_ptr<Road>>;
using TreasureVector = vector<shared_ptr<Treasure>>;

class Map {
  public:
    Map(RNG &rng, LayoutInfo layoutInfo, BlueprintInfo blueprintInfo);

    void initMap();
    void initTiles();

    void generateMap();

    void printMap(int debugLevel);

    void addRegion(shared_ptr<Region> region);
    void addZone(shared_ptr<Zone> zone);
    void addObject(shared_ptr<Object> object);
    void addRoad(shared_ptr<Road> road);
    void addCreature(shared_ptr<Creature> creature);
    void addTreasure(shared_ptr<Treasure> treasure);
    shared_ptr<Tile> getTile(int3 pos);
    LayoutInfo getLayoutInfo();
    BlueprintInfo getBlueprintInfo();
    RegionMap getRegionMap();
    ZoneMap getZoneMap();
    const TileMap &getTileMap();
    ObjectVector getObjectVector();
    RoadVector getRoadVector();
    CreatureVector getCreatureVector();
    TreasureVector getTreasureVector();
    array<int, 4> &getBasicResourceCount();
    RNG &getRNG();
    int getWidth();
    int getHeight();

    void fixNeighbourTiles(const int3 &pos, const int3 &size, int zoneID,
                           const int3 &offset = int3(1, 1, 0));
    bool checkPlacementConflict(const int3 &pos, const int3 &size, const string &types = "BbOTRr",
                                const int3 &offset = int3(0, 0, 0), bool debug = false);

  private:
    RNG &rng;
    LayoutInfo layoutInfo;
    BlueprintInfo blueprintInfo;

    int width, height;
    array<int, 4> basicResourceCount = {0, 0, 0, 0};

    RegionMap regionMap;
    ZoneMap zoneMap;
    TileMap tileMap;
    ObjectVector objectVector;
    CreatureVector creatureVector;
    TreasureVector treasureVector;
    RoadVector roadVector;
};
