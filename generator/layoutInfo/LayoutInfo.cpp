#include "./LayoutInfo.hpp"

LayoutInfo::LayoutInfo() {
    name = "";
    description = "";
    mapSize = "M";
    difficulty = "";
}

string LayoutInfo::getName() { return name; }
string LayoutInfo::getDescription() { return description; }
string LayoutInfo::getMapSize() { return mapSize; }
string LayoutInfo::getDifficulty() { return difficulty; }
vector<RegionInfo> LayoutInfo::getRegionInfoList() { return regionInfoList; }
vector<ConnectionInfo> LayoutInfo::getConnectionInfoList() { return connectionInfoList; }

void LayoutInfo::deserialize(const json &layout) {

    string name = getOrError<string>(layout, "name");
    string description = getOrError<string>(layout, "description");
    string mapSize = layout.value("size", "M");
    string difficulty = getOrError<string>(layout, "difficulty");

    this->name = name;
    this->description = description;
    this->mapSize = mapSize;
    this->difficulty = difficulty;

    const auto &regionList = getOrError<json>(layout, "regions");
    for (const auto &region : regionList) {
        RegionInfo regionInfo;
        regionInfo.deserializeRegion(region);
        regionInfoList.push_back(regionInfo);
    }

    const auto &connectionList = getOrError<json>(layout, "connections");
    for (const auto &connection : connectionList) {
        ConnectionInfo connectionInfo;
        connectionInfo.deserializeConnection(connection);
        connectionInfoList.push_back(connectionInfo);
    }
}

void LayoutInfo::printLayout() {
    cerr << "Template name: " << name << "\n";
    cerr << "Template description: " << description << "\n";
    cerr << "Template map size: " << mapSize << "\n";
    cerr << "Template difficulty: " << difficulty << "\n";

    for (auto region : regionInfoList) {
        region.printRegion();
    }

    for (auto connection : connectionInfoList) {
        cerr << "  Connection:\n";
        connection.printConnection();
    }
}
