#pragma once

#include "../global/Global.hpp"
#include "./ConnectionInfo.hpp"
#include "./RegionInfo.hpp"

class RegionInfo;
class ConnectionInfo;

class LayoutInfo {
  public:
    LayoutInfo();

    string getName();
    string getDescription();
    string getMapSize();
    string getDifficulty();
    vector<RegionInfo> getRegionInfoList();
    vector<ConnectionInfo> getConnectionInfoList();

    void deserialize(const json &layout);
    void printLayout();

  private:
    string name;
    string description;
    string difficulty;
    string mapSize;
    vector<RegionInfo> regionInfoList;
    vector<ConnectionInfo> connectionInfoList;
};
