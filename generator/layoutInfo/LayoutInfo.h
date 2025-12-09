#pragma once

#include "../global/Global.h"
#include "./RegionInfo.h"
#include "./ConnectionInfo.h"

class RegionInfo;
class ConnectionInfo;

class LayoutInfo {
public:

    LayoutInfo();

    void deserialize(const json& layout);
    void printLayout();

private:
    std::string name;
    std::string description;
    std::string difficulty; 
    std::string mapSize;
    std::vector<RegionInfo> regionInfoList;
    std::vector<ConnectionInfo> connectionInfoList;
};
