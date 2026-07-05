#include "./ObjectPlacer.hpp"

ObjectPlacer::ObjectPlacer(Map &map) : map(map) {
    treasureInfo.loadFromJson("./stats/treasureInfo.json");
}

void ObjectPlacer::placeTreasure(string treasureName, int3 pos) {
    Treasure treasure(1, pos, treasureName);
    auto treasurePtr = make_shared<Treasure>(treasure);
    map.addTreasure(treasurePtr);
}

void ObjectPlacer::placeArtifact(ArtifactType artifactType, int3 pos) {
    Artifact artifact(artifactType, pos, "Artifact");
    auto artifactPtr = make_shared<Artifact>(artifact);
    map.addTreasure(artifactPtr);
}

void ObjectPlacer::placeResource(ResourceType resourceType, int3 pos, int quantity) {
    Resource resource(resourceType, quantity, pos, "Resource");
    auto resourcePtr = make_shared<Resource>(resource);
    map.addTreasure(resourcePtr);
}

void ObjectPlacer::placeMine(int3 pos, MineType mineType) {
    Mine mine(mineType, -1, pos, "Mine");
    auto minePtr = make_shared<Mine>(mine);

    map.addObject(minePtr);
    map.fixNeighbourTiles(pos, getMineSize(mineType), getMineRealSize(mineType),
                          map.getTile(pos)->getZoneID());
}

void ObjectPlacer::placeMines() {
    int mapWidth  = map.getWidth();
    int mapHeight = map.getHeight();
    RNG &rng      = map.getRNG();

    int3 mineOffset = int3(1, 1, 0);

    std::map<int, vector<int3>> zoneTiles;

    for (int x = 0; x < mapWidth; x++) {
        for (int y = 0; y < mapHeight; y++) {
            int3 tilePos = int3(x, y, 0);
            auto tile    = map.getTile(tilePos);
            int zoneID   = tile->getZoneID();
            if (tile->isTileType("F")) {
                if (!isInside(2, 2, mapWidth - 2, mapHeight - 2, tilePos) ||
                    map.checkPlacementConflict(tilePos, int3(4, 2, 0), "BbTRr", mineOffset)) {
                    continue;
                }
                zoneTiles[zoneID].push_back(tilePos);
            }
        }
    }

    auto zoneMap = map.getZoneMap();
    for (const auto &[zoneID, zone] : zoneMap) {
        GroupSettingMap &groupSettingMap = map.getGroupSettingMap();
        string zoneType                  = zone->getType();
        string zoneHash                  = map.getZoneHash(zoneID);

        auto minimumMinesCount =
            map.getTemplateInfo().getZoneById(zoneID).getMinimumMines().mineCounts;

        vector<int3> placedMines;
        vector<int3> placedObjects;
        auto objectVector = map.getObjectVector();
        for (const auto &object : objectVector) {
            if (auto town = dynamic_pointer_cast<Town>(object)) {
                if (map.getTile(town->getPosition())->getZoneID() == zoneID) {
                    placedObjects.push_back(town->getPosition());
                }
            }
        }

        vector<int3> minePositions;
        vector<MineType> mineTypeOrder;

        int totalMinesToPlace = 0;
        for (const auto &[mineType, count] : minimumMinesCount) {
            totalMinesToPlace += count;
            for (int i = 0; i < count; i++)
                mineTypeOrder.push_back(mineType);
        }
        rng.shuffle(mineTypeOrder);

        // want to keep sawmill and ore pit on first and second place
        for (size_t i = 0; i < mineTypeOrder.size(); i++) {
            if (mineTypeOrder[i] == MineType::MINE_SAWMILL && i != 0) {
                swap(mineTypeOrder[i], mineTypeOrder[0]);
            } else if (mineTypeOrder[i] == MineType::MINE_ORE_PIT && i != 1) {
                swap(mineTypeOrder[i], mineTypeOrder[1]);
            }
        }

        if (groupSettingMap[zoneHash]->isOrderSet()) {
            mineTypeOrder = groupSettingMap[zoneHash]->getOrder();
        } else {
            groupSettingMap[zoneHash]->setOrder(mineTypeOrder);
        }

        for (int i = 0; i < totalMinesToPlace; i++) {
            float tolerance = (zoneType == "treasure") ? 0.6f : 0.8f;
            int3 minePos    = map.findBestDistributedPosition(
                zoneTiles[zoneID], placedMines, placedObjects, zone->getCenter(), tolerance, -1, 2);

            if (minePos.x == -1)
                break;

            minePositions.push_back(minePos);
            placedMines.push_back(minePos);
        }

        vector<bool> used(minePositions.size(), false);
        int3 current = zone->getCenter();

        for (size_t i = 0; i < mineTypeOrder.size() && i < minePositions.size(); i++) {

            auto passable = [&](const int3 &p) -> bool {
                auto tile = map.getTile(p);
                if (!tile)
                    return false;
                if (tile->isTileType("BbOT"))
                    return false;
                return true;
            };
            auto neighbours = [&](const int3 &p) {
                vector<int3> out;
                for (int k = 0; k < 8; k++) {
                    int3 nxt = p + directions8[k];
                    if (passable(nxt))
                        out.push_back(nxt);
                }
                return out;
            };

            auto bfsDist = bfs_distances<int3>(current, neighbours);

            int bestIdx  = -1;
            int bestDist = numeric_limits<int>::max();

            for (size_t j = 0; j < minePositions.size(); j++) {
                if (used[j])
                    continue;
                auto it = bfsDist.find(minePositions[j]);
                if (it == bfsDist.end())
                    continue; // unreachable
                if (it->second < bestDist) {
                    bestDist = it->second;
                    bestIdx  = j;
                }
            }

            if (bestIdx == -1)
                break;

            used[bestIdx] = true;
            placeMine(minePositions[bestIdx], mineTypeOrder[i]);
            current = minePositions[bestIdx];
        }
    }
}

void ObjectPlacer::placeMineResources() {
    int3 left  = int3(-2, 1, 0);
    int3 right = int3(0, 1, 0);

    for (auto &object : map.getObjectVector()) {
        if (auto mine = dynamic_pointer_cast<Mine>(object)) {

            auto mineType             = mine->getMineType();
            ResourceType resourceType = mineTypeToResourceType(mineType);

            int3 pos = mine->getPosition();
            placeResource(resourceType, pos + left, 0);
            map.getTile(pos + left)->setTileType(TileType::TILE_OCCUPIED);
            placeResource(resourceType, pos + right, 0);
            map.getTile(pos + right)->setTileType(TileType::TILE_OCCUPIED);
        }
    }
}

double ObjectPlacer::evalTreasureCandidate(int3 candidatePosition,
                                           vector<vector<int>> &tilesTreeCount,
                                           vector<int3> &freeTiles, int acceptableBlockedTiles) {
    double score = 0.0;

    for (int i = 0; i < 8; i++) {
        auto neighborPos = candidatePosition + directions8[i];
        if (!map.getTile(neighborPos))
            continue;
        if (map.getTile(neighborPos)->isTileType("OBb")) {
            score += 1.0;
        } else {
            score += tilesTreeCount[neighborPos.x][neighborPos.y];
        }
    }

    auto passableWithCandidate = [&](const int3 &p) -> bool {
        auto tile = map.getTile(p);
        if (!tile)
            return false;
        if (tile->isTileType("BbOT"))
            return false;
        if (abs(p.x - candidatePosition.x) <= 1 && abs(p.y - candidatePosition.y) <= 1)
            return false;
        if (tile->getZoneID() != map.getTile(candidatePosition)->getZoneID())
            return false;
        return true;
    };
    auto passableWithoutCandidate = [&](const int3 &p) -> bool {
        auto tile = map.getTile(p);
        if (!tile)
            return false;
        if (tile->isTileType("BbOT"))
            return false;
        if (tile->getZoneID() != map.getTile(candidatePosition)->getZoneID())
            return false;
        return true;
    };

    auto neighbours8 = [&](const int3 &p) {
        std::array<int3, 8> out;
        for (int i = 0; i < 8; i++)
            out[i] = p + directions8[i];
        return out;
    };

    auto zoneCenter = map.getZoneMap()[map.getTile(candidatePosition)->getZoneID()]->getCenter();
    vector<pair<int, int3>> source = {{2, zoneCenter}};

    if (abs(candidatePosition.x - zoneCenter.x) <= 1 &&
        abs(candidatePosition.y - zoneCenter.y) <= 1)
        return numeric_limits<double>::min();

    auto &ctx = map.getSearchCtx();
    auto claimedTilesWithoutCandidate =
        bfs_claim_xy(ctx, source, neighbours8, passableWithoutCandidate);
    auto claimedTilesWithCandidate = bfs_claim_xy(ctx, source, neighbours8, passableWithCandidate);

    int blockedTiles = 0;
    for (auto &tilePos : freeTiles) {
        if (claimedTilesWithCandidate[ctx.idx(tilePos)] == 0 &&
            claimedTilesWithoutCandidate[ctx.idx(tilePos)] == 2) {
            bool isBlocked = true;
            for (int i = 0; i < 8; i++) {
                auto neighborPos = tilePos + directions8[i];
                if (!map.getTile(neighborPos))
                    continue;
                if (claimedTilesWithCandidate[ctx.idx(neighborPos)] == 2) {
                    isBlocked = false;
                    break;
                }
            }
            if (isBlocked) {
                blockedTiles++;
            }
        }
    }
    if (blockedTiles < acceptableBlockedTiles) {
        score += blockedTiles * 2;
    } else {
        score -= (blockedTiles - acceptableBlockedTiles) * 2;
    }

    return score;
}

void ObjectPlacer::placeTreasures() {
    for (auto &[zoneID, zone] : map.getZoneMap()) {
        auto treasureSettingsVector = map.getTemplateInfo().getZoneById(zoneID).getTreasure();
        int settingsNumber          = treasureSettingsVector.size();
        for (int tier = 0; tier < settingsNumber; tier++) {
            auto treasureTier = treasureSettingsVector[tier];
            placeTreasuresFromTier(zoneID, tier, treasureTier);
        }
    }
}

vector<int3> ObjectPlacer::getPossibleTreasurePositions(int3 candidatePosition) {
    auto passableWithCandidate = [&](const int3 &p) -> bool {
        auto tile = map.getTile(p);
        if (!tile)
            return false;
        if (tile->isTileType("BbOT"))
            return false;
        if (abs(p.x - candidatePosition.x) <= 1 && abs(p.y - candidatePosition.y) <= 1)
            return false;
        if (tile->getZoneID() != map.getTile(candidatePosition)->getZoneID())
            return false;
        return true;
    };
    auto passableWithoutCandidate = [&](const int3 &p) -> bool {
        auto tile = map.getTile(p);
        if (!tile)
            return false;
        if (tile->isTileType("BbOT"))
            return false;
        if (tile->getZoneID() != map.getTile(candidatePosition)->getZoneID())
            return false;
        return true;
    };

    auto neighbours8 = [&](const int3 &p) {
        std::array<int3, 8> out;
        for (int i = 0; i < 8; i++)
            out[i] = p + directions8[i];
        return out;
    };

    int zoneID                     = map.getTile(candidatePosition)->getZoneID();
    auto zoneCenter                = map.getZoneMap()[zoneID]->getCenter();
    vector<pair<int, int3>> source = {{2, zoneCenter}};

    auto &ctx = map.getSearchCtx();
    auto claimedTilesWithoutCandidate =
        bfs_claim_xy_copy(ctx, source, neighbours8, passableWithoutCandidate);
    auto claimedTilesWithCandidate =
        bfs_claim_xy_copy(ctx, source, neighbours8, passableWithCandidate);

    vector<int3> possiblePositions;
    for (int x = 0; x < map.getWidth(); x++) {
        for (int y = 0; y < map.getHeight(); y++) {
            int3 tilePos = int3(x, y, 0);
            if (tilePos == candidatePosition)
                continue;
            if (claimedTilesWithoutCandidate[x][y] == 2 && claimedTilesWithCandidate[x][y] == 0 &&
                map.getTile(tilePos)->isTileType("F") &&
                map.getTile(tilePos)->getZoneID() == zoneID) {
                bool isGood = true;
                for (int i = 0; i < 8; i++) {
                    auto neighborPos = tilePos + directions8[i];
                    if (!map.getTile(neighborPos))
                        continue;
                    if (claimedTilesWithCandidate[neighborPos.x][neighborPos.y] == 2) {
                        isGood = false;
                        break;
                    }
                }
                if (isGood) {
                    possiblePositions.push_back(tilePos);
                }
            }
        }
    }

    return possiblePositions;
}

bool ObjectPlacer::placeTreasuresNearCandidate(int3 candidatePosition, int desiredValue) {
    int currentValue    = 0;
    bool placedAnything = false;

    vector<int3> offsets = {{-1, -1, 0}, {0, -1, 0}, {1, -1, 0}};
    // try to place one building
    vector<MapObjectDefinition> possibleBuildings;
    for (auto &object : treasureInfo.allObjects) {
        if (object.value <= desiredValue && (object.category == ObjectCategory::VISITABLE ||
                                             object.category == ObjectCategory::CREATURE_BANK)) {

            bool canPlace = false;
            for (int i = 0; i < 3; i++) {
                int3 objectPos = candidatePosition + offsets[i] - object.entryPoint;
                if (map.checkPlacementConflict(objectPos, object.size, "BoTRr", int3(1, 1, 0))) {
                    continue;
                }
                canPlace = true;
            }
            if (canPlace) {
                possibleBuildings.push_back(object);
            }
        }
    }

    if (possibleBuildings.size() != 0) {
        auto randomBuilding = map.getRNG().getRandomFromVector(possibleBuildings);

        vector<int3> possibleObjectPos;
        for (int i = 0; i < 3; i++) {
            int3 objectPos = candidatePosition + offsets[i] - randomBuilding.entryPoint;
            if (map.checkPlacementConflict(objectPos, randomBuilding.size, "BoTRr",
                                           int3(1, 1, 0))) {
                continue;
            }
            possibleObjectPos.push_back(objectPos);
        }
        int3 randomObjectPos = map.getRNG().getRandomFromVector(possibleObjectPos);

        placeTreasure(randomBuilding.objectName, randomObjectPos);

        int zoneID = map.getTile(randomObjectPos)->getZoneID();
        map.fixNeighbourTiles(randomObjectPos, randomBuilding.size, randomBuilding.realSize, zoneID,
                              int3(0, 0, 0));
        placedAnything = true;
        currentValue += randomBuilding.value;
    }

    vector<int3> possibleTreasurePositions = getPossibleTreasurePositions(candidatePosition);

    int numberOfTreasurePositions = possibleTreasurePositions.size();
    if (numberOfTreasurePositions == 0)
        return placedAnything;

    while (currentValue < desiredValue) {

        int differenceValue                 = desiredValue - currentValue;
        MapObjectDefinition treasureToPlace = treasureInfo.allObjects[0];
        for (auto &object : treasureInfo.allObjects) {
            if (object.value <= differenceValue && (object.category == ObjectCategory::PICKUP ||
                                                    object.category == ObjectCategory::RESOURCE)) {
                treasureToPlace = object;
            }
        }

        if (possibleTreasurePositions.size() == 0)
            break;
        int3 randomPos = map.getRNG().getRandomFromVector(possibleTreasurePositions);

        auto randomPosIt =
            find(possibleTreasurePositions.begin(), possibleTreasurePositions.end(), randomPos);

        if (randomPosIt != possibleTreasurePositions.end()) {
            possibleTreasurePositions.erase(randomPosIt);
        }

        int quantity = 1;
        if (treasureToPlace.category == ObjectCategory::RESOURCE) {
            int quantityToPlace =
                min(10, (differenceValue + treasureToPlace.value - 1) / treasureToPlace.value);
            placeCorrespondingResource(treasureToPlace.objectName, randomPos, quantityToPlace);
            quantity = quantityToPlace;
        } else {
            placeTreasure(treasureToPlace.objectName, randomPos);
        }

        map.getTile(randomPos)->setTileType(TileType::TILE_OCCUPIED);
        placedAnything = true;

        currentValue += treasureToPlace.value * quantity;
    }
    return placedAnything;
}

void ObjectPlacer::placeCorrespondingResource(string objectName, int3 position, int quantity) {
    ResourceType type = ResourceType::RESOURCE_UNKNOWN;

    if (objectName == "Wood") {
        type = ResourceType::RESOURCE_WOOD;
    } else if (objectName == "Sulfur") {
        type = ResourceType::RESOURCE_SULFUR;
    } else if (objectName == "Crystal") {
        type = ResourceType::RESOURCE_CRYSTAL;
    } else if (objectName == "Gems") {
        type = ResourceType::RESOURCE_GEMS;
    } else if (objectName == "Ore") {
        type = ResourceType::RESOURCE_ORE;
    } else if (objectName == "Mercury") {
        type = ResourceType::RESOURCE_MERCURY;
    } else if (objectName == "Gold") {
        type = ResourceType::RESOURCE_GOLD;
    }

    placeResource(type, position, quantity);
}

void ObjectPlacer::placeTreasuresFromTier(int zoneID, int tier, TreasureTier treasureTier) {
    int mapWidth           = map.getWidth();
    int mapHeight          = map.getHeight();
    string monsterStrength = map.getTemplateInfo().getZoneById(zoneID).getMonsters().strength;

    vector<int3> placedObjects;
    vector<int3> placedTreasures;
    vector<int3> possiblePositions;
    int totalZoneArea = 0;

    for (int x = 0; x < mapWidth; x++) {
        for (int y = 0; y < mapHeight; y++) {
            int3 tilePos = int3(x, y, 0);
            if (map.getTile(tilePos)->getZoneID() == zoneID) {
                if (map.getTile(tilePos)->isTileType("F")) {
                    possiblePositions.push_back(tilePos);
                }
                totalZoneArea++;
            }
        }
    }

    auto objectVector = map.getObjectVector();
    for (const auto &object : objectVector) {
        if (auto mine = dynamic_pointer_cast<Mine>(object)) {
            if (map.getTile(mine->getPosition())->getZoneID() == zoneID) {
                placedObjects.push_back(mine->getPosition());
            }
        } else if (auto town = dynamic_pointer_cast<Town>(object)) {
            if (map.getTile(town->getPosition())->getZoneID() == zoneID) {
                placedObjects.push_back(town->getPosition());
            }
        }
    }
    auto treasureVector = map.getTreasureVector();
    for (const auto &treasure : treasureVector) {
        if (auto resource = dynamic_pointer_cast<Resource>(treasure)) {
            if (map.getTile(resource->getPosition())->getZoneID() == zoneID) {
                placedObjects.push_back(resource->getPosition());
            }
        } else if (auto artifact = dynamic_pointer_cast<Artifact>(treasure)) {
            if (map.getTile(artifact->getPosition())->getZoneID() == zoneID) {
                placedObjects.push_back(artifact->getPosition());
            }
        } else if (auto treasureObj = dynamic_pointer_cast<Treasure>(treasure)) {
            if (map.getTile(treasureObj->getPosition())->getZoneID() == zoneID) {
                placedObjects.push_back(treasureObj->getPosition());
            }
        }
    }

    int density = treasureTier.density;
    int low     = treasureTier.low;
    int high    = treasureTier.high;

    float minDistanceFactor = 10.0f;
    switch (map.getWidth()) {
    case 144:
        minDistanceFactor = 10.0f;
        break;
    case 108:
        minDistanceFactor = 5.0f;
        break;
    case 72:
        minDistanceFactor = 3.0f;
        break;
    default:
        minDistanceFactor = 2.0f;
    }
    float minDistanceRelative = sqrt(totalZoneArea / density / minDistanceFactor);
    float minDistanceTotal    = 3;

    vector<int3> treasureCandidatesPruned;
    vector<int3> treasureCandidates;

    if (tier <= 1) {
        treasureCandidatesPruned = generateTreasureCandidates(possiblePositions, 1);
        treasureCandidates       = generateTreasureCandidates(possiblePositions, 0);

        for (const auto placedObject : placedObjects) {
            filterCandidates(treasureCandidatesPruned, placedObject, minDistanceTotal);
            filterCandidates(treasureCandidates, placedObject, minDistanceTotal);
        }

        // Guard positions cannot be adjacent to road tiles
        auto isAdjacentToRoad = [&](const int3 &pos) {
            for (const auto &dir : directions8) {
                auto n = map.getTile(pos + dir);
                if (n && n->isTileType("r"))
                    return true;
            }
            return false;
        };

        auto eraseRoadAdjacent = [&](vector<int3> &v) {
            v.erase(remove_if(v.begin(), v.end(), isAdjacentToRoad), v.end());
        };

        eraseRoadAdjacent(treasureCandidatesPruned);
        eraseRoadAdjacent(treasureCandidates);

        evaluateCandidates(treasureCandidatesPruned, possiblePositions);
    }

    while (true) {

        int3 candidatePosition = int3(-1, -1, -1);

        if (tier <= 1) {
            if (treasureCandidatesPruned.empty()) {
                candidatePosition =
                    treasureCandidates.empty() ? int3(-1, -1, -1) : treasureCandidates[0];
            } else {
                candidatePosition = treasureCandidatesPruned[0];
            }
        } else {
            candidatePosition = map.findBestDistributedPosition(
                possiblePositions, placedTreasures, placedObjects,
                map.getZoneMap()[zoneID]->getCenter(), 0.8f, minDistanceRelative, minDistanceTotal);
        }

        if (candidatePosition.x == -1)
            break;

        if (tier <= 1) {
            filterCandidates(treasureCandidatesPruned, candidatePosition, minDistanceRelative);
            filterCandidates(treasureCandidates, candidatePosition, minDistanceRelative);
        }

        int valueToPlace = map.getRNG().nextInt(low, high);

        if (tier >= 2) {
            vector<MapObjectDefinition> possibleTreasures;
            for (auto &object : treasureInfo.allObjects) {
                if (object.value <= valueToPlace && (object.category == ObjectCategory::PICKUP)) {
                    possibleTreasures.push_back(object);
                }
            }

            if (possibleTreasures.size() != 0) {
                auto randomTreasure = map.getRNG().getRandomFromVector(possibleTreasures);
                placeTreasure(randomTreasure.objectName, candidatePosition);
                map.getTile(candidatePosition)->setTileType(TileType::TILE_OCCUPIED);
            }

        } else {
            bool placedAnything = placeTreasuresNearCandidate(candidatePosition, valueToPlace);
            if (!placedAnything) {
                continue;
            }
            map.getTile(candidatePosition)->setTileType(TileType::TILE_GUARD);
            int finalValue = valueToPlace * getModifierForTreasure(tier, monsterStrength);
            map.addGuardValue(candidatePosition, finalValue);
        }

        placedTreasures.push_back(candidatePosition);
    }
}

float ObjectPlacer::getModifierForTreasure(int treasureTier, string monsterStrength) {
    int strengthIndex;

    if (monsterStrength == "weak") {
        strengthIndex = 0;
    } else if (monsterStrength == "normal") {
        strengthIndex = 1;
    } else if (monsterStrength == "strong") {
        strengthIndex = 2;
    } else {
        throw std::invalid_argument("Invalid monster strength: " + monsterStrength);
    }

    return valueOfTreasureModifiers[treasureTier][strengthIndex];
}

void ObjectPlacer::filterCandidates(vector<int3> &candidates, int3 placedObject,
                                    float minDistance) {

    auto neighbors8 = [&](const int3 &p) {
        std::array<int3, 8> out;
        for (int i = 0; i < 8; i++)
            out[i] = p + directions8[i];
        return out;
    };
    auto passable = [&](const int3 &p) { return map.getTile(p)->isTileType("FRroG"); };

    auto sources = vector<int3>{placedObject};

    auto &ctx               = map.getSearchCtx();
    auto possibleCandidates = bfs_collect_depth_xy(ctx, sources, minDistance, neighbors8, passable);

    set<int3> possibleCandidatesSet(possibleCandidates.begin(), possibleCandidates.end());

    candidates.erase(
        remove_if(candidates.begin(), candidates.end(),
                  [&](const int3 &candidate) { return possibleCandidatesSet.count(candidate); }),
        candidates.end());
}

void ObjectPlacer::evaluateCandidates(vector<int3> &candidates, vector<int3> &zoneTiles) {
    int mapWidth               = map.getWidth();
    int mapHeight              = map.getHeight();
    int acceptableBlockedTiles = 5;

    auto neighbors4 = [&](const int3 &p) {
        std::array<int3, 4> out;
        for (int i = 0; i < 4; i++)
            out[i] = p + directions4[i];
        return out;
    };

    vector<vector<int>> tilesTreeCount(mapWidth, vector<int>(mapHeight, 0));
    for (const auto &tilePos : zoneTiles) {
        for (const auto &n : neighbors4(tilePos)) {
            if (map.getTile(n) && map.getTile(n)->isTileType("OBb")) {
                tilesTreeCount[tilePos.x][tilePos.y]++;
            }
        }
    }

    vector<pair<double, int3>> candidatesWithEval;
    for (auto &candidate : candidates) {
        candidatesWithEval.push_back(
            {evalTreasureCandidate(candidate, tilesTreeCount, zoneTiles, acceptableBlockedTiles),
             candidate});
    }

    sort(
        candidatesWithEval.begin(), candidatesWithEval.end(),
        [](const pair<double, int3> &a, const pair<double, int3> &b) { return a.first > b.first; });

    vector<int3> finalCandidates;
    for (auto &[eval, candidate] : candidatesWithEval) {
        finalCandidates.push_back(candidate);
    }

    candidates = finalCandidates;
}

vector<int3> ObjectPlacer::generateTreasureCandidates(vector<int3> &zoneTiles,
                                                      int minNeighbourTrees) {
    int mapWidth  = map.getWidth();
    int mapHeight = map.getHeight();

    auto neighbors4 = [&](const int3 &p) {
        std::array<int3, 4> out;
        for (int i = 0; i < 4; i++)
            out[i] = p + directions4[i];
        return out;
    };
    auto neighbors8 = [&](const int3 &p) {
        std::array<int3, 8> out;
        for (int i = 0; i < 8; i++)
            out[i] = p + directions8[i];
        return out;
    };
    auto passable = [&](const int3 &p) { return map.getTile(p)->isTileType("F"); };

    vector<int3> tilesWithinTrees;

    vector<vector<bool>> isSource(mapWidth, vector<bool>(mapHeight, false));
    vector<vector<int>> tilesTreeCount(mapWidth, vector<int>(mapHeight, 0));

    for (const auto &tilePos : zoneTiles) {
        bool alreadyPushed      = false;
        int treeCountDiagonal   = 0;
        int treeCountOrthogonal = 0;
        for (const auto &n : neighbors8(tilePos)) {
            if (map.getTile(n) && map.getTile(n)->isTileType("OBb")) {
                if (abs(n.x - tilePos.x) == 1 && abs(n.y - tilePos.y) == 1) {
                    treeCountDiagonal++;
                } else {
                    treeCountOrthogonal++;
                }
                int sumTree = treeCountDiagonal + treeCountOrthogonal * 2;
                if (!alreadyPushed && sumTree >= minNeighbourTrees) {
                    tilesWithinTrees.push_back(tilePos);
                    isSource[tilePos.x][tilePos.y] = true;
                    alreadyPushed                  = true;
                }

                tilesTreeCount[tilePos.x][tilePos.y]++;
            }
        }
    }

    auto sources = tilesWithinTrees;

    auto possibleCandidates =
        bfs_collect_depth_xy(map.getSearchCtx(), sources, 1, neighbors4, passable);
    vector<int3> candidates;
    for (const auto &candidate : possibleCandidates) {
        if (!isSource[candidate.x][candidate.y]) {
            candidates.push_back(candidate);
        }
    }

    return candidates;
}
