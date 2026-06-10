#pragma once

#include "../global/Global.hpp"

class GroupSetting {
  public:
    GroupSetting() : isOrderSetted(false) {};
    void setOrder(vector<MineType> &order) {
        orderOfMines  = order;
        isOrderSetted = true;
    }
    vector<MineType> &getOrder() { return orderOfMines; }
    bool getIsOrderSetted() { return isOrderSetted; }

  private:
    bool isOrderSetted;
    vector<MineType> orderOfMines;
};
