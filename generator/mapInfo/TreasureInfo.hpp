#pragma once

#include "../global/Global.hpp"
#include "../global/Random.hpp"

enum class ObjectCategory {
    UNKNOWN = 0,
    PICKUP,
    VISITABLE,
    CREATURE_BANK,
    RESOURCE,
    PANDORAS_BOX // TODO: standalone?
};

struct MapObjectDefinition {
    int value;
    int3 size;
    int3 entryPoint;
    string objectName;
    vector<string> realSize;
    ObjectCategory category;
};

class MapObjectRegistry {
  public:
    vector<MapObjectDefinition> allObjects;
    void loadFromJson(const string &filepath);
};
