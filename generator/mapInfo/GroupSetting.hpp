#pragma once

#include "../global/Global.hpp"

class GroupSetting {
  public:
    GroupSetting() : isOrderSet(false) {};
    void setOrder(vector<MineType> &order) {
        orderOfMines  = order;
        isOrderSet = true;
    }
    vector<MineType> &getOrder() { return orderOfMines; }
    bool getIsOrderSet() { return isOrderSet; }

  private:
    bool isOrderSet;
    vector<MineType> orderOfMines;
};
