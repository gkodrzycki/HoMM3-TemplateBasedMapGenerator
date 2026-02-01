#include "./ObjectPlacer.hpp"

ObjectPlacer::ObjectPlacer(Map &map) : map(map) {}

void ObjectPlacer::placeArtifact(ArtifactType artifactType, int3 pos) {

    Artifact artifact(artifactType, pos, "Artifact");
    auto artifactPtr = make_shared<Artifact>(artifact);
    map.addTreasure(artifactPtr);
}

void ObjectPlacer::placeResource(ResourceType resourceType, int3 pos, int quantity) {
    if (quantity <= 0)
        return;

    Resource resource(resourceType, quantity, pos, "Resource");
    auto resourcePtr = make_shared<Resource>(resource);
    map.addTreasure(resourcePtr);
}

void ObjectPlacer::placeCreature(CreatureType creatureType, int3 pos, int quantity) {
    if (quantity <= 0)
        return;

    Creature creature(creatureType, pos, quantity, "COMPLIANT", true, true, "Creature");
    auto creaturePtr = make_shared<Creature>(creature);
    map.addCreature(creaturePtr);
}

void ObjectPlacer::placeBasicMines() {
    auto objectVector = map.getObjectVector();

    int mapWidth  = map.getWidth();
    int mapHeight = map.getHeight();

    for (const auto &object : objectVector) {
        if (auto town = dynamic_pointer_cast<Town>(object)) {
            int3 anchorPoint = object->getPosition();
            anchorPoint.x -= 2;

            pair<int3, int3> BC;
            int maxIter = 10000;
            while (maxIter--) {

                BC          = map.getRNG().getRandomTriangle(anchorPoint, 30);
                auto [B, C] = BC;

                if (!isInside(2, 2, mapWidth - 2, mapHeight - 2, B))
                    continue;
                if (!isInside(2, 2, mapWidth - 2, mapHeight - 2, C))
                    continue;

                if (map.getTile(B)->getZoneID() != map.getTile(anchorPoint)->getZoneID())
                    continue;
                if (map.getTile(C)->getZoneID() != map.getTile(anchorPoint)->getZoneID())
                    continue;

                if (anchorPoint.distance2DSQ(B) <= 5)
                    continue;
                if (anchorPoint.distance2DSQ(C) <= 5)
                    continue;
                if (B.distance2DSQ(C) <= 3)
                    continue;

                if (map.checkPlacementConflict(B, getMineSize(MineType::MINE_SAWMILL)))
                    continue;

                if (map.checkPlacementConflict(C, getMineSize(MineType::MINE_ORE_PIT)))
                    continue;
                // if (B and C inside and good)
                break;
            }

            if (maxIter <= 0)
                throw runtime_error("Failed to place basic mines after maximum iterations");

            auto [B, C] = BC;

            int anchorZoneID                  = map.getTile(anchorPoint)->getZoneID();
            array<int, 4> &basicResourceCount = map.getBasicResourceCount();
            int3 left                         = int3(-2, 1, 0);
            int3 right                        = int3(0, 1, 0);

            Mine mineSawmill(MineType::MINE_SAWMILL, -1, B, "Mine");
            auto mineSawmillPtr = make_shared<Mine>(mineSawmill);
            map.addObject(mineSawmillPtr);
            map.fixNeighbourTiles(B, getMineSize(mineSawmill.getMineType()), anchorZoneID);
            placeResource(ResourceType::RESOURCE_WOOD, B + left, basicResourceCount[0]);
            map.getTile(B + left)->setTileType(TileType::TILE_OCCUPIED);
            placeResource(ResourceType::RESOURCE_WOOD, B + right, basicResourceCount[1]);
            map.getTile(B + right)->setTileType(TileType::TILE_OCCUPIED);

            Mine mineOrePit(MineType::MINE_ORE_PIT, -1, C, "Mine");
            auto mineOrePitPtr = make_shared<Mine>(mineOrePit);
            map.addObject(mineOrePitPtr);
            map.fixNeighbourTiles(C, getMineSize(mineOrePit.getMineType()), anchorZoneID);
            placeResource(ResourceType::RESOURCE_ORE, C + left, basicResourceCount[2]);
            map.getTile(C + left)->setTileType(TileType::TILE_OCCUPIED);
            placeResource(ResourceType::RESOURCE_ORE, C + right, basicResourceCount[3]);
            map.getTile(C + right)->setTileType(TileType::TILE_OCCUPIED);
        }
    }
}

void ObjectPlacer::placeTreasures() {

    int mapWidth  = map.getWidth();
    int mapHeight = map.getHeight();

    std::map<int, vector<pair<int3, shared_ptr<Tile>>>> zoneTiles;

    for (int x = 0; x < mapWidth; x++) {
        for (int y = 0; y < mapHeight; y++) {
            int3 tilePos = int3(x, y, 0);
            auto tile    = map.getTile(tilePos);
            int zoneID   = tile->getZoneID();
            if (tile->isTileType("F")) {
                zoneTiles[zoneID].push_back({tilePos, tile});
            }
        }
    }

    for (auto [id, tiles] : zoneTiles) {
        auto &rng = map.getRNG();
        int numberOfTreasures =
            rng.nextInt(3, 5); // TODO: change to some parameter specified in blueprint

        ArtifactTier tierOfTreasures = static_cast<ArtifactTier>(rng.nextInt(1, 3));

        for (int i = 0; i < numberOfTreasures; i++) {
            bool placed               = false;
            int maxNumberOfIterations = 100;
            while (!placed && maxNumberOfIterations-- >= 0) {
                auto [tilePos, tile] = tiles[rng.nextInt(0, tiles.size() - 1)];
                if (tile->isTileType("F")) { // need to check again because we may claim it :DD
                    tile->setTileType(TileType::TILE_OCCUPIED);

                    auto randomArtifactType = getArtifactFromTier(tierOfTreasures, rng);
                    placeArtifact(randomArtifactType, tilePos);
                    placed = true;
                }
            }
        }
    }
}
