#pragma once

#include "../global/int3.hpp"
#include "./Object.hpp"

class Obstacle : public Object {
  public:
    Obstacle();
    Obstacle(string obstacleName, int3 position, string name);

    void setObstacleName(string obstacleName);

    string getObstacleName();

    void printObject() const override;

  private:
    string obstacleName;
};
