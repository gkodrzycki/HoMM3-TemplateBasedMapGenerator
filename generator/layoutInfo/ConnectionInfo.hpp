#pragma once

#include "../global/Global.hpp"

class ConnectionInfo {

  public:
    ConnectionInfo();

    int getZoneFrom();
    int getZoneDest();

    void deserializeConnection(const json &connection);
    void printConnection();

  private:
    int zoneFrom;
    int zoneDest;
};
