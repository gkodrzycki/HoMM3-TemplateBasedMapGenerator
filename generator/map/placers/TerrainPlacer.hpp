#pragma once

#include "../../global/CellularAutomata.hpp"
#include "../../mapInfo/Obstacle.hpp"
#include "../Map.hpp"

class Map;

class TerrainPlacer {
  public:
    TerrainPlacer(Map &map);
    void generateNoise();
    void placeObstacles();
    void fillClosedRooms();

    bool isFreeRect(int startX, int startY, int w, int h);

  private:
    Map &map;

    // obstacle catalog loaded from stats/obstacleInfo.json, keyed by lower-case terrain
    std::map<string, vector<Obstacle>> cachedObstaclesByTerrain;
    bool obstacleInfoLoaded = false;

    // tracks placed obstacles per exclusion group: group -> list of {anchor, size}
    std::map<string, vector<pair<int3, int3>>> placedByExclusionGroup;

    void loadObstacleInfo();
    const vector<Obstacle> &getObstaclesForTerrain(const string &terrain);

    vector<vector<int3>> collectObstacleComponents();
    void placeObstacleComponent(const vector<int3> &component);
    vector<int3> getTemplateCoveredTiles(const Obstacle &templ, const int3 &anchor);
    bool canPlaceTemplateAtAnchor(const Obstacle &templ, const int3 &anchor,
                                  const unordered_set<int3> &remaining);
};
