#pragma once

#include "../global/Global.hpp"
#include "../global/Random.hpp"

enum class ObjectCategory {
    UNKNOWN = 0,
    PICKUP,
    VISITABLE,
    CREATURE_BANK,
    RESOURCE,
    PANDORAS_BOX // osobno?
};

struct MapObjectDefinition {
    string objectName;
    int value;
    ObjectCategory category;
};

class MapObjectRegistry {
  public:
    vector<MapObjectDefinition> allObjects;
    unordered_map<string, MapObjectDefinition> objectByName;
    void loadFromJson(const string &filepath);
    MapObjectDefinition getRandomObject(ObjectCategory category, int targetValue, int tolerance,
                                        RNG &rng);

    int getObjectValue(const string &objectName);
};
