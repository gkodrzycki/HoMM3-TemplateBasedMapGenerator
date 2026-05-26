#pragma once

#include "../../global/Global.hpp"
#include "../../global/GridSearch.hpp"
#include "../../mapInfo/Artifact.hpp"
#include "../../mapInfo/Creature.hpp"
#include "../../mapInfo/Mine.hpp"
#include "../../mapInfo/Obstacle.hpp"
#include "../../mapInfo/Resource.hpp"
#include "../../mapInfo/RichnessLevel.hpp"
#include "../../mapInfo/TreasureInfo.hpp"
#include "../Map.hpp"

class Map;

class ObjectPlacer {
  public:
    ObjectPlacer(Map &map);

    void placeBasicMines();
    void placeMines();
    void placeMine(int3 pos, MineType mineType);
    void placeMineResources();
    void placeTreasures();
    void placeTreasuresFromTier(int zoneID, int tier, TreasureTier treasureTier);
    vector<int3> generateTreasureCandidates(vector<int3> &zoneTiles, int minNeighbourTrees);
    void filterCandidates(vector<int3> &candidates, int3 placedObject, float minDistance);
    void evaluateCandidates(vector<int3> &candidates, vector<int3> &zoneTiles);

  private:
    Map &map;
    MapObjectRegistry treasureInfo;
    int evalMinePos(int3 minePos, int3 mineSize);
    void placeTreasure(string treasureName, int3 pos);
    void placeResource(ResourceType resourceType, int3 pos, int quantity);
    void placeArtifact(ArtifactType artifactType, int3 pos);
    double evalTreasureCandidate(int3 candidatePosition, vector<vector<int>> &tilesTreeCount,
                                 vector<int3> &freeTiles, int acceptableBlockedTiles);
    void placeTreasuresNearCandidate(int3 candidatePosition, int desiredValue);
    vector<int3> getPossibleTreasurePositions(int3 candidatePosition);
    // int placeTreasuresNearCandidate(int3 candidatePosition, ArtifactTier tierOfTreasures);
    int getNumberOfTreasures(int zoneID);
    double getPercentageOfMaxTreasures(ArtifactTier tierOfTreasures);
    ArtifactTier getTierOfTreasures(int zoneID);
};
