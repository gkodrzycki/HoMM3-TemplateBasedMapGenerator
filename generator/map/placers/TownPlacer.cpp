#include "./TownPlacer.hpp"

TownPlacer::TownPlacer(Map &map) : map(map) {}

bool TownPlacer::hasSafeMargin(int3 pos, int zoneID, int margin) {
    if (pos.x < margin || pos.y < margin || pos.x >= map.getWidth() - margin ||
        pos.y >= map.getHeight() - margin) {
        return false;
    }

    for (int dx = -margin; dx <= margin; ++dx) {
        for (int dy = -margin; dy <= margin; ++dy) {
            auto neighbor = map.getTile(int3(pos.x + dx, pos.y + dy, pos.z));
            if (neighbor && neighbor->getZoneID() != zoneID) {
                return false;
            }
        }
    }

    return true;
}

int3 findBestDistributedPosition(const vector<int3> &freeTiles, const vector<int3> &placedObjects,
                                 const int3 &zoneCenter, RNG &rng, float tolerance = 0.8f) {
    if (freeTiles.empty())
        return int3(-1, -1, -1);

    if (placedObjects.empty()) {
        int minCenterDistSq = numeric_limits<int>::max();

        vector<pair<int3, int>> tileDistances;
        tileDistances.reserve(freeTiles.size());

        for (const auto &pos : freeTiles) {
            int dx     = pos.x - zoneCenter.x;
            int dy     = pos.y - zoneCenter.y;
            int distSq = dx * dx + dy * dy;

            tileDistances.push_back({pos, distSq});

            if (distSq < minCenterDistSq) {
                minCenterDistSq = distSq;
            }
        }

        int centerThresholdSq = minCenterDistSq + 8;

        vector<int3> validCandidates;
        for (const auto &item : tileDistances) {
            if (item.second <= centerThresholdSq) {
                validCandidates.push_back(item.first);
            }
        }

        return rng.getRandomFromVector(validCandidates);
    }

    int maxMinDistSq = -1;
    vector<pair<int3, int>> tileDistances;
    tileDistances.reserve(freeTiles.size());

    for (const auto &pos : freeTiles) {
        int minDistToAnyObjectSq = numeric_limits<int>::max();

        for (const auto &placedPos : placedObjects) {
            int dx     = pos.x - placedPos.x;
            int dy     = pos.y - placedPos.y;
            int distSq = dx * dx + dy * dy;
            if (distSq < minDistToAnyObjectSq) {
                minDistToAnyObjectSq = distSq;
            }
        }

        tileDistances.push_back({pos, minDistToAnyObjectSq});

        if (minDistToAnyObjectSq > maxMinDistSq) {
            maxMinDistSq = minDistToAnyObjectSq;
        }
    }

    int thresholdSq = static_cast<int>(maxMinDistSq * (tolerance * tolerance));

    vector<int3> validCandidates;
    for (const auto &item : tileDistances) {
        if (item.second >= thresholdSq) {
            validCandidates.push_back(item.first);
        }
    }

    return rng.getRandomFromVector(validCandidates);
}

void TownPlacer::placeSpecificTowns(TownSettings townSettings, vector<string> &townTypes,
                                    int zoneID, bool neutral, bool upgraded,
                                    vector<int3> &freeTiles, vector<int3> &placedTowns) {
    int3 offset(1, 1, 0);
    auto &rng       = map.getRNG();
    int ownership   = neutral ? -1 : townSettings.ownership;
    string townName = neutral ? "Neutral Town" : "Player Town";

    int numberOfTowns = upgraded ? townSettings.minimumCastles : townSettings.minimumTowns;

    int3 zoneCenter = map.getZoneMap()[zoneID]->getCenter();
    string zoneType = map.getZoneMap()[zoneID]->getType();

    if (zoneType == "treasure") {
        // For treasure zones treat each border as placed object so every player will have "similar"
        // distance
        for (int x = 0; x < map.getWidth(); x++) {
            for (int y = 0; y < map.getHeight(); y++) {
                int3 tilePos = int3(x, y, 0);
                auto tile    = map.getTile(tilePos);
                if (tile->getZoneID() == zoneID && tile->isTileType("Bb")) {
                    bool onBorder = false;
                    for (int dx = -1; dx <= 1; dx++) {
                        for (int dy = -1; dy <= 1; dy++) {
                            int3 neighboursPos = tilePos + int3(dx, dy, 0);
                            if (map.getTile(neighboursPos)->getZoneID() != zoneID) {
                                onBorder = true;
                                break;
                            }
                        }
                    }
                    if (onBorder) {
                        placedTowns.push_back(tilePos);
                    }
                }
            }
        }
    }

    for (int i = 0; i < numberOfTowns; i++) {

        vector<int3> currentFreeTiles;
        for (const auto &pos : freeTiles) {
            if (map.getTile(pos)->getTileType() == TileType::TILE_FREE) {
                currentFreeTiles.push_back(pos);
            }
        }

        if (currentFreeTiles.empty())
            break;

        float tolerance = (zoneType == "treasure") ? 0.6f : 0.8f;
        int3 townPos =
            findBestDistributedPosition(currentFreeTiles, placedTowns, zoneCenter, rng, tolerance);

        if (townPos.x == -1)
            break;

        auto townType     = rng.getRandomFromVector(townTypes);
        auto townTypeEnum = neutral ? getEnumFromNameOrThrow<Faction>(townType)
                                    : map.getZoneMap()[zoneID]->getFaction();

        Town town(townTypeEnum, ownership, townPos + offset, townName, upgraded);
        auto townPtr = make_shared<Town>(town);
        map.addObject(townPtr);

        map.fixNeighbourTiles(townPtr->getPosition(), townPtr->getSize(), zoneID, offset);
        int3 entrancePos = townPtr->getPosition() - int3(townPtr->getSize().x / 2, 0, 0);
        map.getTile(entrancePos)->setTileType(TileType::TILE_OCCUPIED);

        placedTowns.push_back(townPos);
    }
}

void TownPlacer::placeTowns() {
    ZoneMap zoneMap = map.getZoneMap();

    for (auto [zoneID, zone] : zoneMap) {
        string zoneType = zone->getType();

        auto townTypes = map.getTemplateInfo().getZoneById(zoneID).getTownTypes();
        // Remove factions that are not playable in the base game (Cove, Factory, Bulwark, Conflux)
        townTypes.erase(remove_if(townTypes.begin(), townTypes.end(),
                                  [](const string &f) {
                                      return f == "Cove" || f == "Factory" || f == "Bulwark" ||
                                             f == "Conflux";
                                  }),
                        townTypes.end());

        auto playerTownsSettings  = map.getTemplateInfo().getZoneById(zoneID).getPlayerTowns();
        auto neutralTownsSettings = map.getTemplateInfo().getZoneById(zoneID).getNeutralTowns();

        vector<int3> freeTiles;
        int requiredMargin = 3;

        for (int x = 0; x < map.getWidth(); x++) {
            for (int y = 0; y < map.getHeight(); y++) {
                int3 pos(x, y, 0);
                auto tile = map.getTile(pos);
                if (tile->getZoneID() == zoneID && tile->getTileType() == TileType::TILE_FREE &&
                    hasSafeMargin(pos, zoneID, requiredMargin)) {
                    freeTiles.push_back(pos);
                }
            }
        }

        vector<int3> placedTowns;

        // Player Castles
        placeSpecificTowns(playerTownsSettings, townTypes, zoneID, false, true, freeTiles,
                           placedTowns);
        // Player Towns
        placeSpecificTowns(playerTownsSettings, townTypes, zoneID, false, false, freeTiles,
                           placedTowns);

        // Neutral Castles
        placeSpecificTowns(neutralTownsSettings, townTypes, zoneID, true, true, freeTiles,
                           placedTowns);
        // Neutral Towns
        placeSpecificTowns(neutralTownsSettings, townTypes, zoneID, true, false, freeTiles,
                           placedTowns);
    }
}
