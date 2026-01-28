#pragma once

#include "../global/int3.hpp"
#include "./Object.hpp"

class Treasure : public Object {
  public:
    Treasure();
    Treasure(int quantity, const int3 &position, const string &name);

    void setQuantity(int quantity);
    int getQuantity() const;

    void printObject() const override;

  private:
    int quantity;
};
