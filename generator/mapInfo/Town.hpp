#pragma once

#include "../global/int3.hpp"
#include "./Faction.hpp"
#include "./Object.hpp"

class Town : public Object {
  public:
    Town();
    Town(Faction faction);
    Town(Faction faction, const int &owner, const int3 &position, const string &name);

    void setFaction(Faction faction);
    void setOwner(const int &owner);

    int getOwner();
    Faction getFaction();

    void printObject() const override;

  private:
    Faction faction;

    int owner;
};
