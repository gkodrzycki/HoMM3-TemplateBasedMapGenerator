#pragma once

#include "../global/Global.h"

class ZoneInfo {
public:

    ZoneInfo();

    void deserializeZone(const json& zone);
    void printZone();

private:
    int id;
    std::string size;
    std::string terrain;
    std::string faction;
    std::string owner;
    std::string type;
};