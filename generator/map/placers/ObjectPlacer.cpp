#include "./ObjectPlacer.hpp"
#include "./GuardPlacer.hpp"

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

    int3 sawmillSize = getMineSize(MineType::MINE_SAWMILL);
    int3 orePitSize  = getMineSize(MineType::MINE_ORE_PIT);

    int3 mineOffset = int3(1, 1, 0);

    for (const auto &object : objectVector) {
        if (auto town = dynamic_pointer_cast<Town>(object)) {
            auto townZoneID    = map.getTile(town->getPosition())->getZoneID();
            auto zoneMap       = map.getZoneMap();
            auto zoneType      = zoneMap[townZoneID]->getType();
            auto zoneBlueprint = map.getBlueprintInfo().getTypeBlueprint(zoneType);
            if (!zoneBlueprint.getPlaceBasicMines()) {
                continue;
            }

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

                if (map.checkPlacementConflict(B, sawmillSize, "BbOTRr", mineOffset))
                    continue;

                if (map.checkPlacementConflict(C, orePitSize, "BbOTRr", mineOffset))
                    continue;
                // if (B and C inside and good)
                break;
            }

            if (maxIter <= 0)
                throw runtime_error("Failed to place basic mines after maximum iterations");

            auto [B, C] = BC;

            placeMine(B, zoneBlueprint.getMineGuards()[MineTypeInfo::SAWMILL][0],
                      MineType::MINE_SAWMILL,
                      zoneBlueprint.getMineResourcesCount()[MineTypeInfo::SAWMILL][0]);

            placeMine(C, zoneBlueprint.getMineGuards()[MineTypeInfo::ORE_PIT][0],
                      MineType::MINE_ORE_PIT,
                      zoneBlueprint.getMineResourcesCount()[MineTypeInfo::ORE_PIT][0]);
        }
    }
}

void ObjectPlacer::placeMines() {
    int mapWidth  = map.getWidth();
    int mapHeight = map.getHeight();
    auto &rng     = map.getRNG();

    int3 mineOffset = int3(1, 1, 0);

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

    auto zoneMap = map.getZoneMap();
    for (const auto &[zoneID, zone] : zoneMap) {
        string zoneType = zone->getType();

        ZoneBlueprint zoneBlueprint = map.getBlueprintInfo().getTypeBlueprint(zoneType);
        auto mines                  = zoneBlueprint.getMines();

        for (const auto &[mineTypeInfo, count] : mines) {
            string mineTypeStr = getMineType(mineTypeInfo, rng, zone->getFaction());
            if (mineTypeInfo == MineTypeInfo::RANDOM_MINE) {
                mineTypeStr =
                    getMineType(zoneBlueprint.getRandomMineTypes()[0], rng, zone->getFaction());
            }
            cerr << "Placing " << count << " mines of type " << mineTypeStr << " in zone " << zoneID
                 << endl;

            MineType mineType = getEnumFromNameOrThrow<MineType>("MINE_" + mineTypeStr);
            int3 mineSize     = getMineSize(mineType);

            for (int i = 0; i < count; i++) {
                if (mineTypeInfo == MineTypeInfo::RANDOM_MINE) {
                    mineTypeStr =
                        getMineType(zoneBlueprint.getRandomMineTypes()[i], rng, zone->getFaction());
                    mineType = getEnumFromNameOrThrow<MineType>("MINE_" + mineTypeStr);
                    mineSize = getMineSize(mineType);
                }
                bool placed               = false;
                int maxNumberOfIterations = 10000;
                while (!placed && maxNumberOfIterations-- >= 0) {
                    auto [tilePos, tile] =
                        zoneTiles[zoneID][rng.nextInt(0, zoneTiles[zoneID].size() - 1)];

                    if (!isInside(2, 2, mapWidth - 2, mapHeight - 2, tilePos) ||
                        map.checkPlacementConflict(tilePos, mineSize, "BbOTRr", mineOffset)) {
                        continue;
                    }

                    placeMine(tilePos, zoneBlueprint.getMineGuards()[mineTypeInfo][i], mineType,
                              zoneBlueprint.getMineResourcesCount()[mineTypeInfo][i]);
                    placed = true;
                }
                if (maxNumberOfIterations <= 0)
                    throw runtime_error("Failed to place mine after maximum iterations");
            }
        }
    }
}

void ObjectPlacer::placeMine(int3 pos, Creature guard, MineType mineType,
                             pair<int, int> mineResourcesCount) {
    int3 down = int3(-1, 1, 0);

    auto guardPtr = make_shared<Creature>(guard);
    guardPtr->setPosition(pos + down);

    Mine mine(mineType, -1, pos, "Mine", mineResourcesCount, guardPtr);
    auto minePtr = make_shared<Mine>(mine);

    map.addObject(minePtr);
    map.fixNeighbourTiles(pos, getMineSize(mineType), map.getTile(pos)->getZoneID());
}

void ObjectPlacer::placeMineResources() {
    int3 left  = int3(-2, 1, 0);
    int3 right = int3(0, 1, 0);

    for (auto &object : map.getObjectVector()) {
        if (auto mine = dynamic_pointer_cast<Mine>(object)) {

            auto mineType             = mine->getMineType();
            ResourceType resourceType = mineTypeToResourceType(mineType);

            int3 pos = mine->getPosition();
            placeResource(resourceType, pos + left, mine->getMineResourcesCount().first);
            map.getTile(pos + left)->setTileType(TileType::TILE_OCCUPIED);
            placeResource(resourceType, pos + right, mine->getMineResourcesCount().second);
            map.getTile(pos + right)->setTileType(TileType::TILE_OCCUPIED);
        }
    }
}

int ObjectPlacer::getNumberOfTreasures(int zoneID) {
    auto &rng = map.getRNG();

    int numberOfTreasures;

    BlueprintInfo blueprintInfo = map.getBlueprintInfo();
    auto zoneMap                = map.getZoneMap();
    auto it                     = zoneMap.find(zoneID);
    if (it == zoneMap.end()) {
        throw runtime_error("Zone not found: " + to_string(zoneID));
    }

    string type = it->second->getType();
    try {
        ZoneBlueprint zoneBlueprint = blueprintInfo.getTypeBlueprint(type);
        switch (decodeRichnessLevel(zoneBlueprint.getRichness())) {
        case RichnessLevel::LOW:
            numberOfTreasures = rng.nextInt(3, 5);
            break;
        case RichnessLevel::MEDIUM:
            numberOfTreasures = rng.nextInt(6, 9);
            break;
        case RichnessLevel::HIGH:
            numberOfTreasures = rng.nextInt(10, 15);
            break;
        case RichnessLevel::UNKNOWN:
        default:
            break;
        }
    } catch (const out_of_range &e) {
        throw runtime_error("Blueprint not found for zone type: " + type);
    }

    return numberOfTreasures;
}

ArtifactTier ObjectPlacer::getTierOfTreasures(int zoneID) {
    int tier;

    BlueprintInfo blueprintInfo = map.getBlueprintInfo();
    auto zoneMap                = map.getZoneMap();
    auto it                     = zoneMap.find(zoneID);
    if (it == zoneMap.end()) {
        throw runtime_error("Zone not found: " + to_string(zoneID));
    }

    string type = it->second->getType();
    try {
        ZoneBlueprint zoneBlueprint = blueprintInfo.getTypeBlueprint(type);
        switch (decodeRichnessLevel(zoneBlueprint.getRichness())) {
        case RichnessLevel::LOW:
            tier = 1;
            break;
        case RichnessLevel::MEDIUM:
            tier = 2;
            break;
        case RichnessLevel::HIGH:
            tier = 3;
            break;
        case RichnessLevel::UNKNOWN:
        default:
            break;
        }
    } catch (const out_of_range &e) {
        throw runtime_error("Blueprint not found for zone type: " + type);
    }

    return static_cast<ArtifactTier>(tier);
}

void ObjectPlacer::placeTreasures() {
    int mapWidth  = map.getWidth();
    int mapHeight = map.getHeight();
    auto &rng     = map.getRNG();

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
        int numberOfTreasures        = getNumberOfTreasures(id);
        ArtifactTier tierOfTreasures = getTierOfTreasures(id);

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
