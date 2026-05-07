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

        // ZoneBlueprint zoneBlueprint = map.getBlueprintInfo().getTypeBlueprint(zoneType);
        // auto mines                  = zoneBlueprint.getMines();

        auto minimumMinesCount =
            map.getTemplateInfo().getZoneById(zoneID).getMinimumMines().mineCounts;

        for (const auto &[mineType, count] : minimumMinesCount) {
            // if (mineTypeInfo == MineTypeInfo::RANDOM_MINE) {
            //     mineTypeStr =
            //         getMineType(zoneBlueprint.getRandomMineTypes()[0], rng, zone->getFaction());
            // }
            cerr << "Placing " << count << " mines of type " << getEnumName<MineType>(mineType)
                 << " in zone " << zoneID << endl;

            int3 mineSize = getMineSize(mineType);

            for (int i = 0; i < count; i++) {
                // if (mineTypeInfo == MineTypeInfo::RANDOM_MINE) {
                //     mineTypeStr =
                //         getMineType(zoneBlueprint.getRandomMineTypes()[i], rng,
                //         zone->getFaction());
                //     mineType = getEnumFromNameOrThrow<MineType>("MINE_" + mineTypeStr);
                //     mineSize = getMineSize(mineType);
                // }
                int numberOfIterations = 100;
                int3 bestPos           = int3(-1, -1, -1);
                int bestEvalScore      = -1;

                while (numberOfIterations-- >= 0) {
                    auto [tilePos, tile] =
                        zoneTiles[zoneID][rng.nextInt(0, zoneTiles[zoneID].size() - 1)];

                    if (!isInside(2, 2, mapWidth - 2, mapHeight - 2, tilePos) ||
                        map.checkPlacementConflict(tilePos, mineSize, "BbTRr", mineOffset)) {
                        continue;
                    }

                    int evalScore = evalMinePos(tilePos, mineSize);

                    if (evalScore > bestEvalScore) {
                        bestPos       = tilePos;
                        bestEvalScore = evalScore;
                    }
                }

                if (bestEvalScore == -1) {
                    throw runtime_error(
                        "Failed to place mine after maximum iterations using seed " +
                        to_string(map.getRNG().getOriginalSeed()));
                }
                pair<int, int> mineResourcesCount = {1, 0}; // TODO get from blueprint

                placeMine(bestPos, mineType, mineResourcesCount);
            }
        }
    }
}

void ObjectPlacer::placeMine(int3 pos, MineType mineType, pair<int, int> mineResourcesCount) {
    Mine mine(mineType, -1, pos, "Mine", mineResourcesCount);
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
    int mapWidth               = map.getWidth();
    int mapHeight              = map.getHeight();
    auto &rng                  = map.getRNG();
    int numberOfCandidates     = 50;
    int acceptableBlockedTiles = 5;

    std::map<int, vector<int3>> zoneTiles;

    for (int x = 0; x < mapWidth; x++) {
        for (int y = 0; y < mapHeight; y++) {
            int3 tilePos = int3(x, y, 0);
            int zoneID   = map.getTile(tilePos)->getZoneID();
            if (map.getTile(tilePos)->isTileType("F")) {
                zoneTiles[zoneID].push_back(tilePos);
            }
        }
    }

    auto neighbors4 = [&](const int3 &p) {
        std::array<int3, 4> out;
        for (int i = 0; i < 4; i++)
            out[i] = p + directions4[i];
        return out;
    };
    auto passable = [&](const int3 &p) { return map.getTile(p)->isTileType("F"); };

    std::map<int, vector<int3>> tilesWithinTrees;

    vector<vector<bool>> isSource(mapWidth, vector<bool>(mapHeight, false));
    vector<vector<int>> tilesTreeCount(mapWidth, vector<int>(mapHeight, 0));
    for (const auto &[zoneID, tiles] : zoneTiles) {
        for (const auto &tilePos : tiles) {
            bool alreadyPushed = false;
            for (const auto &n : neighbors4(tilePos)) {
                if (map.getTile(n) && map.getTile(n)->isTileType("OBb")) {
                    if (!alreadyPushed) {
                        tilesWithinTrees[zoneID].push_back(tilePos);
                        isSource[tilePos.x][tilePos.y] = true;
                    }
                    alreadyPushed = true;

                    tilesTreeCount[tilePos.x][tilePos.y]++;
                }
            }
        }
    }

    // for (const auto &[zoneID, tiles] : zoneTiles) {
    //     cerr << "Zone " << zoneID << " has " << tiles.size() << " free tiles, "
    //          << tilesWithinTrees[zoneID].size() << " of which are within trees\n";

    //     for (const auto &tilePos : tilesWithinTrees[zoneID]) {
    //         // mark in map debugly those tiles
    //         map.getTile(tilePos)->setTileType(TileType::TILE_DEBUG);
    //     }
    // }

    for (auto [id, tiles] : zoneTiles) {
        int numberOfTreasurePoints   = getNumberOfTreasures(id);
        ArtifactTier tierOfTreasures = getTierOfTreasures(id);

        auto sources = tilesWithinTrees[id];
        auto possibleCandidates =
            bfs_collect_depth_xy(mapWidth, mapHeight, sources, 1, neighbors4, passable);

        vector<int3> candidates;
        for (const auto &candidate : possibleCandidates) {
            if (!isSource[candidate.x][candidate.y]) {
                candidates.push_back(candidate);
            }
        }

        vector<pair<double, int3>> candidatesWithEval;
        for (int i = 0; i < numberOfCandidates; i++) {
            int3 candidatePosition = rng.getRandomFromVector(candidates);
            candidatesWithEval.push_back({evalTreasureCandidate(candidatePosition, tilesTreeCount,
                                                                tiles, acceptableBlockedTiles),
                                          candidatePosition});
        }

        sort(candidatesWithEval.begin(), candidatesWithEval.end(),
             [](const pair<double, int3> &a, const pair<double, int3> &b) {
                 return a.first > b.first;
             });

        // for (auto &[eval, candidatePosition] : candidatesWithEval) {
        //     cerr << "Candidate at " << candidatePosition.toString() << " has eval " << eval <<
        //     endl; map.getTile(candidatePosition)->setTileType(TileType::TILE_DEBUG);
        // }

        vector<int3> pickedTreasurePoints;
        for (int i = 0; i < (int)candidatesWithEval.size(); i++) {
            if (numberOfTreasurePoints <= 0)
                break;

            auto candidatePosition = candidatesWithEval[i].second;

            bool tooClose = false;
            for (const auto &pickedPos : pickedTreasurePoints) {
                if (candidatePosition.distance2DMH(pickedPos) <= 5) {
                    tooClose = true;
                    break;
                }
            }
            if (tooClose)
                continue;

            placeTreasuresNearCandidate(candidatePosition, tierOfTreasures);
            map.getTile(candidatePosition)->setTileType(TileType::TILE_GUARD);
            pickedTreasurePoints.push_back(candidatePosition);
            numberOfTreasurePoints--;
        }
    }
}

void ObjectPlacer::placeTreasuresNearCandidate(int3 candidatePosition,
                                               ArtifactTier tierOfTreasures) {

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

    int zoneID                     = map.getTile(candidatePosition)->getZoneID();
    auto zoneCenter                = map.getZoneMap()[zoneID]->getCenter();
    vector<pair<int, int3>> source = {{2, zoneCenter}};

    auto claimedTiles =
        bfs_claim_xy(map.getWidth(), map.getHeight(), source, neighbours8, passable);

    vector<pair<double, int3>> possiblePlacementWithScore;
    for (int x = 0; x < map.getWidth(); x++) {
        for (int y = 0; y < map.getHeight(); y++) {
            int3 tilePos = int3(x, y, 0);
            if (tilePos == candidatePosition)
                continue;
            if (claimedTiles[tilePos.x][tilePos.y] == 0 && map.getTile(tilePos)->isTileType("F") &&
                map.getTile(tilePos)->getZoneID() == zoneID) {
                double score = 0.0;

                score += floor(tilePos.distance2DSQ(candidatePosition)) * 2;
                if (tilePos.distance2DMH(candidatePosition) >= 5) {
                    break;
                }
                for (int i = 0; i < 8; i++) {
                    auto neighborPos = tilePos + directions8[i];
                    if (map.getTile(neighborPos) && map.getTile(neighborPos)->isTileType("OBb")) {
                        score += 1.0;
                    }
                }

                possiblePlacementWithScore.push_back({score, tilePos});
            }
        }
    }

    sort(
        possiblePlacementWithScore.begin(), possiblePlacementWithScore.end(),
        [](const pair<double, int3> &a, const pair<double, int3> &b) { return a.first > b.first; });

    int maxTreasuresToPlace =
        getPercentageOfMaxTreasures(tierOfTreasures) * possiblePlacementWithScore.size();

    set<int3> alreadyTaken;
    for (int i = 0; i < (int)possiblePlacementWithScore.size(); i++) {
        if (maxTreasuresToPlace <= 0)
            break;

        if (alreadyTaken.count(possiblePlacementWithScore[i].second))
            continue;

        auto candidatePos       = possiblePlacementWithScore[i].second;
        auto randomArtifactType = getArtifactFromTier(tierOfTreasures, map.getRNG());
        placeArtifact(randomArtifactType, candidatePos);
        map.getTile(candidatePos)->setTileType(TileType::TILE_OCCUPIED);
        maxTreasuresToPlace--;

        alreadyTaken.insert(candidatePos);
        for (int j = 0; j < (int)possiblePlacementWithScore.size(); j++) {
            if (i == j)
                continue;
            auto otherCandidatePos = possiblePlacementWithScore[j].second;
            if (alreadyTaken.count(otherCandidatePos))
                continue;
            bool candidateIsNearOtherCandidate = false;
            for (int k = 0; k < 8; k++) {
                auto neighborPos = otherCandidatePos + directions8[k];
                if (neighborPos == candidatePos) {
                    possiblePlacementWithScore[j].first += 2;
                    candidateIsNearOtherCandidate = true;
                    break;
                }
            }
            if (candidateIsNearOtherCandidate) {
                i = -1;
                break;
            }
        }

        sort(possiblePlacementWithScore.begin(), possiblePlacementWithScore.end(),
             [](const pair<double, int3> &a, const pair<double, int3> &b) {
                 return a.first > b.first;
             });
    }

    // int blockedTiles = 0;
    // for (auto &tilePos : tiles) {
    //     if (abs(tilePos.x - candidatePosition.x) <= 1 &&
    //         abs(tilePos.y - candidatePosition.y) <= 1) {
    //         continue;
    //     }
    //     if (claimedTiles[tilePos.x][tilePos.y] == 0) {
    //         blockedTiles++;
    //     }
    // }
    // vector<int3> offsets = {int3(-1, -1, 0), int3(0, -1, 0), int3(1, -1, 0), int3(-1, 0, 0),
    //                         int3(1, 0, 0),   int3(-1, 1, 0), int3(0, 1, 0),  int3(1, 1, 0)};

    // for (const auto &offset : offsets) {
    //     int3 pos = treasurePoint + offset;
    //     if (map.getTile(pos) && map.getTile(pos)->isTileType("F")) {
    //         auto randomArtifactType = getArtifactFromTier(tierOfTreasures, map.getRNG());
    //         placeArtifact(randomArtifactType, pos);
    //         map.getTile(pos)->setTileType(TileType::TILE_OCCUPIED);
    //     }
    // }
}
