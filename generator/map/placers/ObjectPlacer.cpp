#include "./ObjectPlacer.hpp"

ObjectPlacer::ObjectPlacer(Map &map) : map(map) {
    treasureInfo.loadFromJson("./treasureInfo.json");
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

// void ObjectPlacer::placeBasicMines() {
//     auto objectVector = map.getObjectVector();

//     int mapWidth  = map.getWidth();
//     int mapHeight = map.getHeight();

//     int3 sawmillSize = getMineSize(MineType::MINE_SAWMILL);
//     int3 orePitSize  = getMineSize(MineType::MINE_ORE_PIT);

//     int3 mineOffset = int3(1, 1, 0);

//     for (const auto &object : objectVector) {
//         if (auto town = dynamic_pointer_cast<Town>(object)) {
//             auto townZoneID    = map.getTile(town->getPosition())->getZoneID();
//             auto zoneMap       = map.getZoneMap();
//             auto zoneType      = zoneMap[townZoneID]->getType();
//             auto zoneBlueprint = map.getBlueprintInfo().getTypeBlueprint(zoneType);
//             if (!zoneBlueprint.getPlaceBasicMines()) {
//                 continue;
//             }

//             int3 anchorPoint = object->getPosition();
//             anchorPoint.x -= 2;

//             pair<int3, int3> BC;
//             int maxIter = 10000;
//             while (maxIter--) {

//                 BC          = map.getRNG().getRandomTriangle(anchorPoint, 30);
//                 auto [B, C] = BC;

//                 if (!isInside(2, 2, mapWidth - 2, mapHeight - 2, B))
//                     continue;
//                 if (!isInside(2, 2, mapWidth - 2, mapHeight - 2, C))
//                     continue;

//                 if (map.getTile(B)->getZoneID() != map.getTile(anchorPoint)->getZoneID())
//                     continue;
//                 if (map.getTile(C)->getZoneID() != map.getTile(anchorPoint)->getZoneID())
//                     continue;

//                 if (anchorPoint.distance2DSQ(B) <= 5)
//                     continue;
//                 if (anchorPoint.distance2DSQ(C) <= 5)
//                     continue;
//                 if (B.distance2DSQ(C) <= 3)
//                     continue;

//                 if (map.checkPlacementConflict(B, sawmillSize, "BbOTRr", mineOffset))
//                     continue;

//                 if (map.checkPlacementConflict(C, orePitSize, "BbOTRr", mineOffset))
//                     continue;
//                 // if (B and C inside and good)
//                 break;
//             }

//             if (maxIter <= 0)
//                 throw runtime_error(
//                     "Failed to place basic mines after maximum iterations using seed " +
//                     to_string(map.getRNG().getOriginalSeed()));

//             auto [B, C] = BC;

//             placeMine(B, zoneBlueprint.getMineGuards()[MineTypeInfo::SAWMILL][0],
//                       MineType::MINE_SAWMILL,
//                       zoneBlueprint.getMineResourcesCount()[MineTypeInfo::SAWMILL][0]);

//             placeMine(C, zoneBlueprint.getMineGuards()[MineTypeInfo::ORE_PIT][0],
//                       MineType::MINE_ORE_PIT,
//                       zoneBlueprint.getMineResourcesCount()[MineTypeInfo::ORE_PIT][0]);
//         }
//     }
// }

int ObjectPlacer::evalMinePos(int3 minePos, int3 mineSize) {
    int score = 0;

    for (int x = minePos.x - mineSize.x / 2; x <= minePos.x + mineSize.x / 2; x++) {
        for (int y = minePos.y - mineSize.y / 2; y <= minePos.y + mineSize.y / 2; y++) {
            auto tile = map.getTile(int3(x, y, 0));
            if (tile != nullptr && tile->isTileType("F")) {
                score += 2;
            }
        }
    }

    for (int x = minePos.x - mineSize.x / 2 - 1; x <= minePos.x + mineSize.x / 2 + 1; x++) {
        auto upperTile = map.getTile(int3(x, minePos.y - mineSize.y / 2 - 1, 0));
        if (upperTile != nullptr && upperTile->isTileType("O"))
            score += 1;
    }

    auto leftTile =
        map.getTile(int3(minePos.x - mineSize.x / 2 - 1, minePos.y - mineSize.y / 2, 0));
    auto rightTile =
        map.getTile(int3(minePos.x + mineSize.x / 2 + 1, minePos.y - mineSize.y / 2, 0));

    if (leftTile != nullptr && leftTile->isTileType("O"))
        score += 1;

    if (rightTile != nullptr && rightTile->isTileType("O"))
        score += 1;

    return score;
}

void ObjectPlacer::placeMines() {
    int mapWidth  = map.getWidth();
    int mapHeight = map.getHeight();

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
        string zoneType = zone->getType();

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

        for (const auto &[mineType, count] : minimumMinesCount) {
            cerr << "Placing " << count << " mines of type " << getEnumName<MineType>(mineType)
                 << " in zone " << zoneID << endl;

            for (int i = 0; i < count; i++) {
                float tolerance = (zoneType == "treasure") ? 0.6f : 0.8f;
                int3 minePos =
                    map.findBestDistributedPosition(zoneTiles[zoneID], placedMines, placedObjects,
                                                    zone->getCenter(), tolerance, -1, 2);

                if (minePos.x == -1)
                    break;

                placeMine(minePos, mineType);
                placedMines.push_back(minePos);
            }
        }
    }
}

void ObjectPlacer::placeMine(int3 pos, MineType mineType) {
    Mine mine(mineType, -1, pos, "Mine");
    auto minePtr = make_shared<Mine>(mine);

    map.addObject(minePtr);
    map.fixNeighbourTiles(pos, getMineSize(mineType), getMineRealSize(mineType),
                          map.getTile(pos)->getZoneID());
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

int ObjectPlacer::getNumberOfTreasures(int zoneID) {
    int numberOfTreasures;

    TemplateInfo templateInfo = map.getTemplateInfo();
    auto zoneMap              = map.getZoneMap();
    auto it                   = zoneMap.find(zoneID);
    if (it == zoneMap.end()) {
        throw runtime_error("Zone not found: " + to_string(zoneID));
    }

    string type = it->second->getType();
    try {
        auto treasureSettings = templateInfo.getZoneById(zoneID).getTreasure();
        auto treasureSetting  = treasureSettings[1]; // TODO: utilize low/medium/high tiers instead
                                                     // of just taking the medium one

        numberOfTreasures =
            treasureSetting
                .density; // default to density if we don't want to use low/medium/high tiers
        // switch (decodeRichnessLevel(zoneBlueprint.getRichness())) {
        // case RichnessLevel::LOW:
        //     numberOfTreasures = rng.nextInt(3, 5);
        //     break;
        // case RichnessLevel::MEDIUM:
        //     numberOfTreasures = rng.nextInt(6, 9);
        //     break;
        // case RichnessLevel::HIGH:
        //     numberOfTreasures = rng.nextInt(10, 15);
        //     break;
        // case RichnessLevel::UNKNOWN:
        // default:
        //     break;
        // }
    } catch (const out_of_range &e) {
        throw runtime_error("Blueprint not found for zone type: " + type);
    }

    return numberOfTreasures;
}

double ObjectPlacer::getPercentageOfMaxTreasures(ArtifactTier tierOfTreasures) {
    switch (tierOfTreasures) {
    case ArtifactTier::LOW:
        return 0.3;
    case ArtifactTier::MID:
        return 0.5;
    case ArtifactTier::HIGH:
        return 0.7;
    default:
        return 0.5;
    }
}

ArtifactTier ObjectPlacer::getTierOfTreasures(int zoneID) {
    int tier;

    TemplateInfo templateInfo = map.getTemplateInfo();
    auto zoneMap              = map.getZoneMap();
    auto it                   = zoneMap.find(zoneID);
    if (it == zoneMap.end()) {
        throw runtime_error("Zone not found: " + to_string(zoneID));
    }

    string type = it->second->getType();
    try {

        auto treasureSettings = templateInfo.getZoneById(zoneID).getTreasure();
        auto treasureSetting  = treasureSettings[1]; // TODO: utilize low/medium/high tiers instead
                                                     // of just taking the medium one

        if (treasureSetting.low < 5000) {
            tier = 1;
        } else if (treasureSetting.low < 10000) {
            tier = 2;
        } else {
            tier = 3;
        }

    } catch (const out_of_range &e) {
        throw runtime_error("Blueprint not found for zone type: " + type);
    }

    return static_cast<ArtifactTier>(tier);
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

    auto passable = [&](const int3 &p) -> bool {
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

    auto claimedTiles =
        bfs_claim_xy(map.getWidth(), map.getHeight(), source, neighbours8, passable);

    int blockedTiles = 0;
    for (auto &tilePos : freeTiles) {
        if (abs(tilePos.x - candidatePosition.x) <= 1 &&
            abs(tilePos.y - candidatePosition.y) <= 1) {
            continue;
        }
        if (claimedTiles[tilePos.x][tilePos.y] == 0) {
            bool isBlocked = true;
            for (int i = 0; i < 8; i++) {
                auto neighborPos = tilePos + directions8[i];
                if (!map.getTile(neighborPos))
                    continue;
                if (claimedTiles[neighborPos.x][neighborPos.y] == 2) {
                    isBlocked = false;
                    break;
                }
            }
            if (isBlocked)
                blockedTiles++;
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
    // int zoneID, int tier, TreasureTier treasureTier
    for (auto &[zoneID, zone] : map.getZoneMap()) {
        auto treasureSettingsVector = map.getTemplateInfo().getZoneById(zoneID).getTreasure();
        int settingsNumber          = treasureSettingsVector.size();
        for (int tier = 0; tier < settingsNumber; tier++) {
            auto treasureTier = treasureSettingsVector[tier];
            placeTreasuresFromTier(zoneID, tier, treasureTier);
        }
    }
    return;

    // std::map<int, vector<int3>> zoneTiles;

    // for (int x = 0; x < mapWidth; x++) {
    //     for (int y = 0; y < mapHeight; y++) {
    //         int3 tilePos = int3(x, y, 0);
    //         int zoneID   = map.getTile(tilePos)->getZoneID();
    //         if (map.getTile(tilePos)->isTileType("F")) {
    //             zoneTiles[zoneID].push_back(tilePos);
    //         }
    //     }
    // }

    // auto neighbors4 = [&](const int3 &p) {
    //     std::array<int3, 4> out;
    //     for (int i = 0; i < 4; i++)
    //         out[i] = p + directions4[i];
    //     return out;
    // };
    // auto passable = [&](const int3 &p) { return map.getTile(p)->isTileType("F"); };

    // std::map<int, vector<int3>> tilesWithinTrees;

    // vector<vector<bool>> isSource(mapWidth, vector<bool>(mapHeight, false));
    // vector<vector<int>> tilesTreeCount(mapWidth, vector<int>(mapHeight, 0));
    // for (const auto &[zoneID, tiles] : zoneTiles) {
    //     for (const auto &tilePos : tiles) {
    //         bool alreadyPushed = false;
    //         for (const auto &n : neighbors4(tilePos)) {
    //             if (map.getTile(n) && map.getTile(n)->isTileType("OBb")) {
    //                 if (!alreadyPushed) {
    //                     tilesWithinTrees[zoneID].push_back(tilePos);
    //                     isSource[tilePos.x][tilePos.y] = true;
    //                 }
    //                 alreadyPushed = true;

    //                 tilesTreeCount[tilePos.x][tilePos.y]++;
    //             }
    //         }
    //     }
    // }

    // // for (const auto &[zoneID, tiles] : zoneTiles) {
    // //     cerr << "Zone " << zoneID << " has " << tiles.size() << " free tiles, "
    // //          << tilesWithinTrees[zoneID].size() << " of which are within trees\n";

    // //     for (const auto &tilePos : tilesWithinTrees[zoneID]) {
    // //         // mark in map debugly those tiles
    // //         map.getTile(tilePos)->setTileType(TileType::TILE_DEBUG);
    // //     }
    // // }

    // for (auto [id, tiles] : zoneTiles) {
    //     int numberOfTreasurePoints   = getNumberOfTreasures(id);
    //     ArtifactTier tierOfTreasures = getTierOfTreasures(id);

    //     auto sources = tilesWithinTrees[id];
    //     auto possibleCandidates =
    //         bfs_collect_depth_xy(mapWidth, mapHeight, sources, 1, neighbors4, passable);

    //     vector<int3> candidates;
    //     for (const auto &candidate : possibleCandidates) {
    //         if (!isSource[candidate.x][candidate.y]) {
    //             candidates.push_back(candidate);
    //         }
    //     }

    //     vector<pair<double, int3>> candidatesWithEval;
    //     for (int i = 0; i < numberOfCandidates; i++) {
    //         int3 candidatePosition = rng.getRandomFromVector(candidates);
    //         candidatesWithEval.push_back({evalTreasureCandidate(candidatePosition,
    //         tilesTreeCount,
    //                                                             tiles, acceptableBlockedTiles),
    //                                       candidatePosition});
    //     }

    //     sort(candidatesWithEval.begin(), candidatesWithEval.end(),
    //          [](const pair<double, int3> &a, const pair<double, int3> &b) {
    //              return a.first > b.first;
    //          });

    //     // for (auto &[eval, candidatePosition] : candidatesWithEval) {
    //     //     cerr << "Candidate at " << candidatePosition.toString() << " has eval " << eval <<
    //     //     endl; map.getTile(candidatePosition)->setTileType(TileType::TILE_DEBUG);
    //     // }

    //     vector<int3> pickedTreasurePoints;
    //     for (int i = 0; i < (int)candidatesWithEval.size(); i++) {
    //         if (numberOfTreasurePoints <= 0)
    //             break;

    //         auto candidatePosition = candidatesWithEval[i].second;

    //         if (candidatePosition.distance2DMH(
    //                 map.getZoneMap()[map.getTile(candidatePosition)->getZoneID()]->getCenter())
    //                 <=
    //             2) {
    //             continue;
    //         }

    //         bool tooClose = false;
    //         for (const auto &pickedPos : pickedTreasurePoints) {
    //             if (candidatePosition.distance2DMH(pickedPos) <= 5) {
    //                 tooClose = true;
    //                 break;
    //             }
    //         }
    //         if (tooClose)
    //             continue;

    //         int placedTreasures = placeTreasuresNearCandidate(candidatePosition,
    //         tierOfTreasures); if (placedTreasures == 0) {
    //             continue;
    //         }
    //         map.getTile(candidatePosition)->setTileType(TileType::TILE_GUARD);
    //         pickedTreasurePoints.push_back(candidatePosition);
    //         numberOfTreasurePoints--;
    //     }
    // }
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

    auto claimedTilesWithoutCandidate = bfs_claim_xy(map.getWidth(), map.getHeight(), source,
                                                     neighbours8, passableWithoutCandidate);

    auto claimedTilesWithCandidate =
        bfs_claim_xy(map.getWidth(), map.getHeight(), source, neighbours8, passableWithCandidate);

    vector<int3> possiblePositions;
    for (int x = 0; x < map.getWidth(); x++) {
        for (int y = 0; y < map.getHeight(); y++) {
            int3 tilePos = int3(x, y, 0);
            if (claimedTilesWithoutCandidate[tilePos.x][tilePos.y] == 2 &&
                claimedTilesWithCandidate[tilePos.x][tilePos.y] == 0 &&
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

void ObjectPlacer::placeTreasuresNearCandidate(int3 candidatePosition, int desiredValue) {
    int currentValue = 0;

    // try to place one building
    vector<MapObjectDefinition> possibleBuildings;
    for (auto &object : treasureInfo.allObjects) {
        if (object.value <= desiredValue && (object.category == ObjectCategory::VISITABLE ||
                                             object.category == ObjectCategory::CREATURE_BANK)) {
            possibleBuildings.push_back(object);
        }
    }

    if (possibleBuildings.size() != 0) {
        auto randomBuilding = map.getRNG().getRandomFromVector(possibleBuildings);
        placeTreasure(randomBuilding.objectName, candidatePosition);
        map.getTile(candidatePosition)->setTileType(TileType::TILE_OCCUPIED);
        currentValue += randomBuilding.value;
    }

    vector<int3> possibleTreasurePositions = getPossibleTreasurePositions(candidatePosition);

    for (auto &treasurePos : possibleTreasurePositions) {
        placeResource(ResourceType::RESOURCE_GOLD, treasurePos, 1);
        map.getTile(treasurePos)->setTileType(TileType::TILE_OCCUPIED);
    }

    while (currentValue < desiredValue) {
        break;
    }
}

// int ObjectPlacer::placeTreasuresNearCandidate(int3 candidatePosition,
//                                               ArtifactTier tierOfTreasures) {

//     auto passable = [&](const int3 &p) -> bool {
//         auto tile = map.getTile(p);
//         if (!tile)
//             return false;
//         if (tile->isTileType("BbOT"))
//             return false;
//         if (abs(p.x - candidatePosition.x) <= 1 && abs(p.y - candidatePosition.y) <= 1)
//             return false;
//         if (tile->getZoneID() != map.getTile(candidatePosition)->getZoneID())
//             return false;
//         return true;
//     };

//     auto neighbours8 = [&](const int3 &p) {
//         std::array<int3, 8> out;
//         for (int i = 0; i < 8; i++)
//             out[i] = p + directions8[i];
//         return out;
//     };

//     int zoneID                     = map.getTile(candidatePosition)->getZoneID();
//     auto zoneCenter                = map.getZoneMap()[zoneID]->getCenter();
//     vector<pair<int, int3>> source = {{2, zoneCenter}};

//     auto claimedTiles =
//         bfs_claim_xy(map.getWidth(), map.getHeight(), source, neighbours8, passable);

//     vector<pair<double, int3>> possiblePlacementWithScore;
//     for (int x = 0; x < map.getWidth(); x++) {
//         for (int y = 0; y < map.getHeight(); y++) {
//             int3 tilePos = int3(x, y, 0);
//             if (tilePos == candidatePosition)
//                 continue;
//             if (claimedTiles[tilePos.x][tilePos.y] == 0 && map.getTile(tilePos)->isTileType("F")
//             &&
//                 map.getTile(tilePos)->getZoneID() == zoneID) {
//                 double score = 0.0;

//                 score += floor(tilePos.distance2DSQ(candidatePosition)) * 2;
//                 if (tilePos.distance2DMH(candidatePosition) >= 5) {
//                     continue;
//                 }
//                 for (int i = 0; i < 8; i++) {
//                     auto neighborPos = tilePos + directions8[i];
//                     if (!map.getTile(neighborPos))
//                         continue;
//                     if (claimedTiles[neighborPos.x][neighborPos.y] == 2) {
//                         score = -1;
//                         break;
//                     }
//                     if (map.getTile(neighborPos)->isTileType("OBb")) {
//                         score += 1.0;
//                     }
//                 }

//                 if (score <= 0)
//                     continue;
//                 possiblePlacementWithScore.push_back({score, tilePos});
//             }
//         }
//     }

//     if (candidatePosition == int3(106, 26, 0)) {
//         for (int y = 0; y < map.getHeight(); y++) {
//             for (int x = 0; x < map.getWidth(); x++) {
//                 if (claimedTiles[x][y] == 0) {
//                     printColor(RED, "0");
//                 } else if (claimedTiles[x][y] == 1) {
//                     printColor(YELLOW, "1");
//                 } else if (claimedTiles[x][y] == 2) {
//                     printColor(GREEN, "2");
//                 }
//             }
//             cerr << endl;
//         }

//         cerr << "\n\n";
//     }

//     sort(
//         possiblePlacementWithScore.begin(), possiblePlacementWithScore.end(),
//         [](const pair<double, int3> &a, const pair<double, int3> &b) { return a.first > b.first;
//         });

//     int maxTreasuresToPlace =
//         getPercentageOfMaxTreasures(tierOfTreasures) * possiblePlacementWithScore.size();

//     set<int3> alreadyTaken;
//     int placedTreasures = 0;
//     for (int i = 0; i < (int)possiblePlacementWithScore.size(); i++) {
//         if (maxTreasuresToPlace <= 0)
//             break;

//         if (alreadyTaken.count(possiblePlacementWithScore[i].second))
//             continue;

//         auto candidatePos       = possiblePlacementWithScore[i].second;
//         auto randomArtifactType = getArtifactFromTier(tierOfTreasures, map.getRNG());
//         placeArtifact(randomArtifactType, candidatePos);
//         map.getTile(candidatePos)->setTileType(TileType::TILE_OCCUPIED);
//         maxTreasuresToPlace--;
//         placedTreasures++;

//         alreadyTaken.insert(candidatePos);
//         for (int j = 0; j < (int)possiblePlacementWithScore.size(); j++) {
//             if (i == j)
//                 continue;
//             auto otherCandidatePos = possiblePlacementWithScore[j].second;
//             if (alreadyTaken.count(otherCandidatePos))
//                 continue;
//             bool candidateIsNearOtherCandidate = false;
//             for (int k = 0; k < 8; k++) {
//                 auto neighborPos = otherCandidatePos + directions8[k];
//                 if (neighborPos == candidatePos) {
//                     possiblePlacementWithScore[j].first += 2;
//                     candidateIsNearOtherCandidate = true;
//                     break;
//                 }
//             }
//             if (candidateIsNearOtherCandidate) {
//                 i = -1;
//                 break;
//             }
//         }

//         sort(possiblePlacementWithScore.begin(), possiblePlacementWithScore.end(),
//              [](const pair<double, int3> &a, const pair<double, int3> &b) {
//                  return a.first > b.first;
//              });
//     }

//     return placedTreasures;
// }

void ObjectPlacer::placeTreasuresFromTier(int zoneID, int tier, TreasureTier treasureTier) {
    int mapWidth  = map.getWidth();
    int mapHeight = map.getHeight();

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
    cerr << "Zone " << zoneID << " has total area " << totalZoneArea << " and "
         << possiblePositions.size() << " possible positions for treasures\n";

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
                cerr << "Found already placed artifact at " << artifact->getPosition().toString()
                     << " in zone " << zoneID << endl;
                placedObjects.push_back(artifact->getPosition());
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
    cerr << "Placing treasures in zone " << zoneID << " with tier " << tier << " and density "
         << density << " and minDistance " << minDistanceRelative << endl;

    vector<int3> treasureCandidatesPruned;
    vector<int3> treasureCandidates;

    if (tier <= 1) {
        treasureCandidatesPruned = generateTreasureCandidates(possiblePositions, 7);
        treasureCandidates       = generateTreasureCandidates(possiblePositions, 4);

        for (const auto placedObject : placedObjects) {
            filterCandidates(treasureCandidatesPruned, placedObject, minDistanceTotal);
            filterCandidates(treasureCandidates, placedObject, minDistanceTotal);
        }

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

        // candidataPosition to guard?

        int valueToPlace = map.getRNG().nextInt(low, high);

        if (tier >= 2) {
            // continue;
        } else {

            placeTreasuresNearCandidate(candidatePosition, valueToPlace);
        }

        auto randomArtifactType = ArtifactType::ARTIFACT_ANGEL_FEATHER_ARROWS;
        if (tier == 0) {
            randomArtifactType = ArtifactType::ARTIFACT_SPYGLASS;
        } else if (tier == 1) {
            randomArtifactType = ArtifactType::ARTIFACT_BOOTS_OF_POLARITY;
        } else if (tier == 2) {
            randomArtifactType = ArtifactType::ARTIFACT_DRAGON_SCALE_SHIELD;
        }
        placeArtifact(randomArtifactType, candidatePosition);
        map.getTile(candidatePosition)->setTileType(TileType::TILE_OCCUPIED);

        placedTreasures.push_back(candidatePosition);
    }
}

void ObjectPlacer::filterCandidates(vector<int3> &candidates, int3 placedObject,
                                    float minDistance) {
    int mapWidth  = map.getWidth();
    int mapHeight = map.getHeight();

    auto neighbors8 = [&](const int3 &p) {
        std::array<int3, 8> out;
        for (int i = 0; i < 8; i++)
            out[i] = p + directions8[i];
        return out;
    };
    auto passable = [&](const int3 &p) { return map.getTile(p)->isTileType("FRroG"); };

    auto sources = vector<int3>{placedObject};

    auto possibleCandidates =
        bfs_collect_depth_xy(mapWidth, mapHeight, sources, minDistance, neighbors8, passable);

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
    int mapWidth               = map.getWidth();
    int mapHeight              = map.getHeight();
    auto &rng                  = map.getRNG();
    int numberOfCandidates     = 100;
    int acceptableBlockedTiles = 5;

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
        bfs_collect_depth_xy(mapWidth, mapHeight, sources, 1, neighbors4, passable);

    vector<int3> candidates;
    for (const auto &candidate : possibleCandidates) {
        if (!isSource[candidate.x][candidate.y]) {
            candidates.push_back(candidate);
        }
    }

    return candidates;

    // cerr << "Generated " << candidates.size() << " treasure candidates\n";

    // // TODO: filter candidates that are too closed to objects
    // for (const auto &placedObject : placedObjects) {
    //     candidates.erase(remove_if(candidates.begin(), candidates.end(),
    //                                [&](const int3 &candidate) {
    //                                    int distance = candidate.distance2DSQ(placedObject);
    //                                    return distance * distance <= minDistance;
    //                                }),
    //                      candidates.end());
    // }

    // cerr << "Filtered candidates based on distance to placed objects, remaining candidates: " <<
    // candidates.size() << endl;

    // vector<pair<double, int3>> candidatesWithEval;
    // for (int i = 0; i < min(numberOfCandidates, (int)candidates.size()); i++) {
    //     int3 candidatePosition = rng.getRandomFromVector(candidates);
    //     candidatesWithEval.push_back({evalTreasureCandidate(candidatePosition, tilesTreeCount,
    //                                                         zoneTiles, acceptableBlockedTiles),
    //                                     candidatePosition});
    // }

    // sort(candidatesWithEval.begin(), candidatesWithEval.end(),
    //         [](const pair<double, int3> &a, const pair<double, int3> &b) {
    //             return a.first > b.first;
    //         });

    // vector<int3> finalCandidates;
    // for (auto &[eval, candidate]: candidatesWithEval) {
    //     finalCandidates.push_back(candidate);
    // }

    // if (finalCandidates.empty()) {
    //     return {int3(-1, -1, -1)};
    // }

    // return finalCandidates;
}
