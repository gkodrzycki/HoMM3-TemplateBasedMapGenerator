#pragma once

#include "../global/int3.hpp"
#include "./Creature.hpp"
#include "./MineType.hpp"
#include "./Object.hpp"

class Mine : public Object {
  public:
    Mine();
    Mine(const MineType &mineType, int ownerID, const int3 &position, const string &name);

    void setMineType(const MineType &mineType);
    const MineType getMineType() const;

    void setOwner(const int &ownerID);
    int getOwner() const;

    void printObject() const override;

  private:
    MineType mineType;
    int ownerID;
};
