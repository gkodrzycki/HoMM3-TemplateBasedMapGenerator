#include "./ObjectPlacer.hpp"

ObjectPlacer::ObjectPlacer(Map &map) : map(map) {}

void ObjectPlacer::placeResource(ResourceType resourceType, int3 pos, int quantity) {
    if (quantity <= 0)
        return;

    Resource resource(resourceType, quantity, pos, "Resource");
    auto resourcePtr = make_shared<Resource>(resource);
    map.addResource(resourcePtr);
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
            int maxIter = 1000;
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
            placeResource(ResourceType::RESOURCE_WOOD, B + right, basicResourceCount[1]);

            Mine mineOrePit(MineType::MINE_ORE_PIT, -1, C, "Mine");
            auto mineOrePitPtr = make_shared<Mine>(mineOrePit);
            map.addObject(mineOrePitPtr);
            map.fixNeighbourTiles(C, getMineSize(mineOrePit.getMineType()), anchorZoneID);
            placeResource(ResourceType::RESOURCE_ORE, C + left, basicResourceCount[2]);
            placeResource(ResourceType::RESOURCE_ORE, C + right, basicResourceCount[3]);
        }
    }
}
