#pragma once

#include "../global/CellularAutomata.hpp"
#include "../global/Global.hpp"
#include "../global/Random.hpp"
#include "../mapInfo/Creature.hpp"
#include "../mapInfo/Object.hpp"
#include "../mapInfo/Resource.hpp"
#include "../mapInfo/Tile.hpp"
#include "../mapInfo/Zone.hpp"
#include "../templateInfo/TemplateInfo.hpp"
#include "./placers/BorderPlacer.hpp"
#include "./placers/ConnectionPlacer.hpp"
#include "./placers/GuardPlacer.hpp"
#include "./placers/ObjectPlacer.hpp"
#include "./placers/TerrainPlacer.hpp"
#include "./placers/TownPlacer.hpp"
#include "./placers/ZonePlacer.hpp"

using ZoneMap        = map<int, shared_ptr<Zone>>;
using TileMap        = map<int, map<int, shared_ptr<Tile>>>;
using ObjectVector   = vector<shared_ptr<Object>>;
using CreatureVector = vector<shared_ptr<Creature>>;
using RoadVector     = vector<shared_ptr<Road>>;
using TreasureVector = vector<shared_ptr<Treasure>>;
using MonolithVector = vector<pair<shared_ptr<Object>, shared_ptr<Object>>>;

class Map {
  public:
    Map(RNG &rng, TemplateInfo templateInfo);

    void initMap();
    void initTiles();

    void generateMap();

    void placeDebugObjects();
    void printMap(int debugLevel);

    void addZone(shared_ptr<Zone> zone);
    void addObject(shared_ptr<Object> object);
    void addRoad(shared_ptr<Road> road);
    void addCreature(shared_ptr<Creature> creature);
    void addTreasure(shared_ptr<Treasure> treasure);
    void addMonoliths(shared_ptr<Object> monolithFrom, shared_ptr<Object> monolithDest);
    void fixReachability();
    TemplateInfo getTemplateInfo();
    shared_ptr<Tile> getTile(int3 pos);
    ZoneMap getZoneMap();
    const TileMap &getTileMap();
    ObjectVector getObjectVector();
    RoadVector getRoadVector();
    CreatureVector getCreatureVector();
    TreasureVector getTreasureVector();
    MonolithVector getMonolithVector();
    RNG &getRNG();
    int getWidth();
    int getHeight();

    void fixNeighbourTiles(const int3 &pos, const int3 &size, int zoneID,
                           const int3 &offset = int3(1, 1, 0));
    bool checkPlacementConflict(const int3 &pos, const int3 &size, const string &types = "BbOTRr",
                                const int3 &offset = int3(0, 0, 0), bool debug = false);
    int3 findBestDistributedPosition(const vector<int3> &freeTiles,
                                     const vector<int3> &placedObjects, const int3 &zoneCenter,
                                     RNG &rng, float tolerance = 0.8f);

  private:
    pair<int, int> chooseMapSize(int minimumSize, int maximumSize);

    RNG &rng;
    TemplateInfo templateInfo;

    int width, height;

    ZoneMap zoneMap;
    TileMap tileMap;
    ObjectVector objectVector;
    CreatureVector creatureVector;
    TreasureVector treasureVector;
    RoadVector roadVector;
    MonolithVector monolithVector;
};
