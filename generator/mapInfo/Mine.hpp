#pragma once

#include "../global/int3.hpp"
#include "./MineType.hpp"
#include "./Object.hpp"

class Mine : public Object {
  public:
    Mine();
    Mine(const MineType &mineType, const int3 &position, const string &name);

    void setMineType(const MineType &mineType);

    const MineType getMineType() const;

    void printObject() const override;

  private:
    MineType mineType;
};
