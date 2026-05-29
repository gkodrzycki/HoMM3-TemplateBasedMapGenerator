#include "TreasureInfo.hpp"

int3 parseInt3(const string &entryStr) {
    size_t commaPos = entryStr.find(',');
    if (commaPos != string::npos) {
        int x = stoi(entryStr.substr(0, commaPos));
        int y = stoi(entryStr.substr(commaPos + 1));
        return int3(x, y, 0);
    }
    return int3(0, 0, 0);
}

ObjectCategory determineCategory(const string &objectName) {
    if (objectName == "cyclopsStockpile" || objectName == "dwarvenTreasury" ||
        objectName == "griffinConservatory" || objectName == "impCache" ||
        objectName == "medusaStore" || objectName == "nagaBank" || objectName == "dragonFlyHive" ||
        objectName == "crypt" || objectName == "dragonUtopia") {
        return ObjectCategory::CREATURE_BANK;
    } else if (objectName == "Ore" || objectName == "Gold" || objectName == "Gems" ||
               objectName == "Crystal" || objectName == "Mercury" || objectName == "Sulfur" ||
               objectName == "Wood") {
        return ObjectCategory::RESOURCE;
    } else if (objectName == "randomArtifactTreasure" || objectName == "randomArtifactMinor" ||
               objectName == "randomArtifactMajor" || objectName == "randomArtifactRelic" ||
               objectName == "campfire" || objectName == "treasureChest") {
        return ObjectCategory::PICKUP;
    } else if (objectName == "pandoraBox") {
        return ObjectCategory::PANDORAS_BOX;
    } else {
        return ObjectCategory::VISITABLE;
    }
}

void MapObjectRegistry::loadFromJson(const string &filepath) {
    ifstream file(filepath);
    if (!file.is_open()) {
        throw runtime_error("Failed to open file: " + filepath);
    }
    json jsonData;
    file >> jsonData;

    for (const auto &item : jsonData) {
        MapObjectDefinition obj;
        obj.objectName = getOrError<string>(item, "objectName");
        obj.value      = getOrError<int>(item, "value");
        obj.size       = parseInt3(getOrDefault<string>(item, "size", "1,1"));
        obj.entryPoint = parseInt3(getOrDefault<string>(item, "entryPoint", "0,0"));

        if (item.contains("realSize")) {
            for (const auto &row : item["realSize"]) {
                obj.realSize.push_back(row.get<string>());
            }
        } else {
            for (int i = 0; i < obj.size.y; ++i) {
                obj.realSize.push_back(string(obj.size.x, '1'));
            }
        }

        obj.category = determineCategory(obj.objectName);
        allObjects.push_back(obj);
    }

    sort(allObjects.begin(), allObjects.end(),
         [](const MapObjectDefinition &a, const MapObjectDefinition &b) {
             return a.value < b.value;
         });
}
