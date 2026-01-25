#include "./GuardPlacer.hpp"

GuardPlacer::GuardPlacer(Map &map) : map(map) {}

void GuardPlacer::placeBorderGuards() {
    ObjectVector objectVector = map.getObjectVector();

    for (const auto &object : objectVector) {
        if (auto road = dynamic_pointer_cast<Road>(object)) {
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

            Creature guard = Creature(CreatureType::PIKEMAN, currGuardPos, 1, "COMPLIANT", true,
                                      true, "Creature");
            auto guardPtr  = make_shared<Creature>(guard);
            map.addCreature(guardPtr);
        }
    }
}

void GuardPlacer::placeGuards() { placeBorderGuards(); }
