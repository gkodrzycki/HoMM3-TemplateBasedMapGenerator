#pragma once

#include "../global/Global.hpp"
#include "./ZoneBlueprint.hpp"

class ZoneBlueprint;

class BlueprintInfo {
  public:
    BlueprintInfo();

    vector<ZoneBlueprint> getBlueprints();
    ZoneBlueprint getTypeBlueprint(const string &type);

    void deserialize(const json &blueprint);
    void printBlueprint();

  private:
    vector<ZoneBlueprint> blueprints;
};
