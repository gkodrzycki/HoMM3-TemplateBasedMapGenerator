#pragma once

#include "../global/CellularAutomata.hpp"
#include "../global/Global.hpp"
#include "../global/GridSearch.hpp"
#include "../global/Random.hpp"
#include "../mapInfo/Creature.hpp"
#include "../mapInfo/GroupSetting.hpp"
#include "../mapInfo/Object.hpp"
#include "../mapInfo/PandoraBox.hpp"
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

using HashMap          = map<int, string>;
using GroupSettingMap  = map<string, shared_ptr<GroupSetting>>;
using GuardValueMap    = map<int3, int>;
using ZoneMap          = map<int, shared_ptr<Zone>>;
using TileMap          = vector<vector<shared_ptr<Tile>>>;
using ObjectVector     = vector<shared_ptr<Object>>;
using CreatureVector   = vector<shared_ptr<Creature>>;
using RoadVector       = vector<shared_ptr<Road>>;
using TreasureVector   = vector<shared_ptr<Treasure>>;
using PandoraBoxVector = vector<shared_ptr<PandoraBox>>;
using MonolithVector   = vector<pair<shared_ptr<Object>, shared_ptr<Object>>>;

class Map {
  public:
    Map(RNG &rng, TemplateInfo templateInfo);

    void initMap();
    void initTiles();

    void generateMap();

    void placeDebugObjects();
    void printMap(int debugLevel);
    void printRealSizeMap();

    void addHash(int zoneID, string hash);
    void addZone(shared_ptr<Zone> zone);
    void addObject(shared_ptr<Object> object);
    void addRoad(shared_ptr<Road> road);
    void addCreature(shared_ptr<Creature> creature);
    void addTreasure(shared_ptr<Treasure> treasure);
    void addPandoraBox(shared_ptr<PandoraBox> pandoraBox);
    void addMonoliths(shared_ptr<Object> monolithFrom, shared_ptr<Object> monolithDest);
    void addGuardValue(int3 pos, int value);
    void fixReachability();
    string getZoneHash(int zoneID);
    GroupSettingMap &getGroupSettingMap();
    TemplateInfo getTemplateInfo();
    Tile *getTile(int3 pos);
    ZoneMap getZoneMap();
    const TileMap &getTileMap();
    ObjectVector getObjectVector();
    RoadVector getRoadVector();
    CreatureVector getCreatureVector();
    TreasureVector getTreasureVector();
    PandoraBoxVector getPandoraBoxVector();
    MonolithVector getMonolithVector();
    HashMap getHashMap();
    GridSearchContext &getSearchCtx();
    GuardValueMap getGuardValueMap();
    RNG &getRNG();
    int getWidth();
    int getHeight();

    void fixNeighbourTiles(const int3 &pos, const int3 &size, const vector<string> &realSize,
                           int zoneID, const int3 &offset = int3(1, 1, 0));
    bool checkPlacementConflict(const int3 &pos, const int3 &size, const string &types = "BbOTRr",
                                const int3 &offset = int3(0, 0, 0), bool debug = false);
    int3 findBestDistributedPosition(const vector<int3> &freeTiles,
                                     const vector<int3> &placedSameObjects,
                                     const vector<int3> &placedAllObjects, const int3 &zoneCenter,
                                     float tolerance = 0.8f, int minDistanceRelative = -1,
                                     int minDistanceTotal = -1);

  private:
    pair<int, int> chooseMapSize(int minimumSize, int maximumSize);

    RNG &rng;
    TemplateInfo templateInfo;

    GridSearchContext searchCtx;

    int width, height;

    HashMap hashMap;
    GroupSettingMap groupSettingMap;
    ZoneMap zoneMap;
    TileMap tileMap;
    ObjectVector objectVector;
    CreatureVector creatureVector;
    TreasureVector treasureVector;
    PandoraBoxVector pandoraBoxVector;
    RoadVector roadVector;
    MonolithVector monolithVector;
    GuardValueMap guardValueMap;
};
