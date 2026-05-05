#pragma once

#include "../../global/Global.hpp"
#include "../../global/GridSearch.hpp"
#include "../../mapInfo/Artifact.hpp"
#include "../../mapInfo/Creature.hpp"
#include "../../mapInfo/Mine.hpp"
#include "../../mapInfo/Obstacle.hpp"
#include "../../mapInfo/Resource.hpp"
#include "../../mapInfo/RichnessLevel.hpp"
#include "../Map.hpp"

class Map;

class ObjectPlacer {
  public:
    ObjectPlacer(Map &map);

    void placeBasicMines();
    void placeMines();
    void placeMine(int3 pos, MineType mineType, pair<int, int> mineResourcesCount);
    void placeMineResources();
    void placeTreasures();

  private:
    Map &map;
    int evalMinePos(int3 minePos, int3 mineSize);
    void placeResource(ResourceType resourceType, int3 pos, int quantity);
    void placeArtifact(ArtifactType artifactType, int3 pos);
    double evalTreasureCandidate(int3 candidatePosition, vector<vector<int>> &tilesTreeCount,
                                 vector<int3> &freeTiles, int acceptableBlockedTiles);
    void placeTreasuresNearCandidate(int3 candidatePosition, ArtifactTier tierOfTreasures);
    int getNumberOfTreasures(int zoneID);
    double getPercentageOfMaxTreasures(ArtifactTier tierOfTreasures);
    ArtifactTier getTierOfTreasures(int zoneID);
};
