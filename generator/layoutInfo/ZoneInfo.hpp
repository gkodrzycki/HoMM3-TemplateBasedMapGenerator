#pragma once

#include "../global/Global.hpp"

class ZoneInfo {
  public:
    ZoneInfo();

    int getID();
    string getSize();
    string getTerrain();
    string getFaction();
    string getOwner();
    string getType();

    void deserializeZone(const json &zone);
    void printZone();

  private:
    int id;
    string size;
    string terrain;
    string faction;
    string owner;
    string type;
};