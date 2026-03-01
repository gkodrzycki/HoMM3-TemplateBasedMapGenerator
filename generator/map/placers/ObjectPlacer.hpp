#pragma once

#include "../../global/Global.hpp"
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
    void placeMineResources();
    void placeTreasures();

  private:
    Map &map;

    void placeCreature(CreatureType creatureType, int3 pos, int quantity);
    void placeResource(ResourceType resourceType, int3 pos, int quantity);
    void placeArtifact(ArtifactType artifactType, int3 pos);
    int getNumberOfTreasures(int zoneID);
    ArtifactTier getTierOfTreasures(int zoneID);
};
