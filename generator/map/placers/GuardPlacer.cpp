#include "./GuardPlacer.hpp"

#include "../../global/GridSearch.hpp"

GuardPlacer::GuardPlacer(Map &map) : map(map) {}

void GuardPlacer::placeBorderGuards() {
    RoadVector roadVector = map.getRoadVector();
    ZoneMap zoneMap       = map.getZoneMap();
    int W                 = map.getWidth();
    int H                 = map.getHeight();

    for (const auto &road : roadVector) {
        vector<int3> path = road->getPath();
        if (path.empty())
            continue;

        // Determine the two zones connected by this road
        int zoneA = -1, zoneB = -1;
        for (const auto &pos : path) {
            auto tile = map.getTile(pos);
            if (!tile)
                continue;
            int zid = tile->getZoneID();
            if (zoneA == -1)
                zoneA = zid;
            else if (zid != zoneA && zoneB == -1)
                zoneB = zid;
        }

        if (zoneA == -1 || zoneB == -1)
            continue;

        int3 centerA = zoneMap[zoneA]->getCenter();
        int3 centerB = zoneMap[zoneB]->getCenter();

        auto neighbors8 = [](const int3 &p) {
            array<int3, 8> out;
            for (int i = 0; i < 8; i++)
                out[i] = p + directions8[i];
            return out;
        };

        vector<int3> candidates;
        set<int3> pathSet(path.begin(), path.end());

        for (int x = 0; x < W; x++) {
            for (int y = 0; y < H; y++) {
                int3 pos(x, y, 0);
                auto tile = map.getTile(pos);
                if (!tile || tile->isTileType("B"))
                    continue;
                int zid = tile->getZoneID();
                if (zid != zoneA && zid != zoneB)
                    continue;
                bool nearBorder = false;
                for (const auto &dir : directions8) {
                    auto neighborTile = map.getTile(pos + dir);
                    if (neighborTile && neighborTile->isTileType("B")) {
                        nearBorder = true;
                        break;
                    }
                }
                if (nearBorder)
                    candidates.push_back(pos);
            }
        }

        stable_partition(candidates.begin(), candidates.end(),
                         [&](const int3 &p) { return pathSet.count(p) > 0; });

        bool placed = false;
        for (const auto &candidatePos : candidates) {
            // Skip candidates whose 3x3 block would overlap zone centers
            if (abs(candidatePos.x - centerA.x) <= 1 && abs(candidatePos.y - centerA.y) <= 1)
                continue;
            if (abs(candidatePos.x - centerB.x) <= 1 && abs(candidatePos.y - centerB.y) <= 1)
                continue;

            auto passable = [&](const int3 &p) -> bool {
                auto tile = map.getTile(p);
                if (!tile)
                    return false;
                if (tile->isTileType("B"))
                    return false;
                // 3x3 blocked area around the guard candidate
                if (abs(p.x - candidatePos.x) <= 1 && abs(p.y - candidatePos.y) <= 1)
                    return false;
                return true;
            };

            auto reachPath = bfs_path_xy(W, H, centerA, centerB, neighbors8, passable);

            if (reachPath.empty()) {
                GuardHandler guardHandler(map.getRNG());
                auto guardPtr = guardHandler.createGuard(GuardTypeHandler::BORDER, candidatePos);
                if (guardPtr != nullptr) {
                    map.addCreature(guardPtr);
                }
                placed = true;
                break;
            }
        }

        if (!placed) {
            throw runtime_error("Failed to find valid guard position between zones " +
                                to_string(zoneA) + " and " + to_string(zoneB));
        }
    }
}

void GuardPlacer::placeMineGuards() {
    ObjectVector objectVector = map.getObjectVector();

    for (const auto &object : objectVector) {
        if (auto mine = dynamic_pointer_cast<Mine>(object)) {

            auto guardPtr = mine->getGuardPtr();

            if (guardPtr != nullptr) {
                map.addCreature(guardPtr);
            }
        }
    }
}

void GuardPlacer::placeGuards() {
    placeBorderGuards();
    placeMineGuards();
}
