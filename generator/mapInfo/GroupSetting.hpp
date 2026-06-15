#pragma once

#include "../global/Global.hpp"
#include "./MineType.hpp"

class GroupSetting {
  public:
    GroupSetting() : _isOrderSet(false) {};
    void setOrder(vector<MineType> &order) {
        orderOfMines = order;
        _isOrderSet  = true;
    }
    vector<MineType> &getOrder() { return orderOfMines; }
    bool isOrderSet() { return _isOrderSet; }

  private:
    bool _isOrderSet;
    vector<MineType> orderOfMines;
};
