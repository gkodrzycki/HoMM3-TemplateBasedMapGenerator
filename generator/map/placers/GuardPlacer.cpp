#include "./GuardPlacer.hpp"

GuardPlacer::GuardPlacer(Map &map) : map(map) {}

void GuardPlacer::placeBorderGuards() {
    RoadVector roadVector = map.getRoadVector();

    for (const auto &road : roadVector) {
        vector<int3> path   = road->getPath();
        int3 currGuardPos   = path[0];
        int mostBorderTiles = 0;
        for (auto pos : path) {
            int borderTileCount = 0;
            for (auto dir : directions8) {
                int3 neighborPos = pos + dir;
                auto tilePtr     = map.getTile(neighborPos);
                if (tilePtr != nullptr && tilePtr->isTileType("B")) {
                    borderTileCount++;
                }
            }

            if (borderTileCount > mostBorderTiles) {
                mostBorderTiles = borderTileCount;
                currGuardPos    = pos;
            }
        }

        GuardHandler guardHandler(map.getRNG());
        auto guardPtr = guardHandler.createGuard(GuardTypeHandler::BORDER, currGuardPos);
        if (guardPtr != nullptr) {
            map.addCreature(guardPtr);
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
