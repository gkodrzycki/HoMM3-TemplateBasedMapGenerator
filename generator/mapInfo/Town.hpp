#pragma once

#include "../global/int3.hpp"
#include "./Faction.hpp"
#include "./Object.hpp"

class Town : public Object {
  public:
    Town();
    Town(Faction faction);
    Town(Faction faction, const string &owner, const int3 &position, const string &name);

    void setFaction(Faction faction);
    void setOwner(const string &owner);
    void setHasFort(bool hasFort);

    string getOwner();
    Faction getFaction();
    bool getHasFort() const;

    void printObject() const override;

  private:
    Faction faction;

    string owner;
    bool hasFort;
};
