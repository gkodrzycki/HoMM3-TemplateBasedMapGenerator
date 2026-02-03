#pragma once

#include "../global/Global.hpp"

class ZoneBlueprint {
  public:
    ZoneBlueprint();

    string getType();
    string getRichness();

    void deserializeZoneBlueprint(const json &zone);
    void printZoneBlueprint();

  private:
    string type;
    string richness;
};
