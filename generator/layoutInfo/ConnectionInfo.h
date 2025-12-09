#pragma once

#include "../global/Global.h"


class ConnectionInfo {

public:
    ConnectionInfo();
    
    void deserializeConnection(const json& connection);
    void printConnection();

private:
    int zoneFrom;
    int zoneDest;

};