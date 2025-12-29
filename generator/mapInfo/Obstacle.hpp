#pragma once

#include "../global/int3.hpp"
#include "./Object.hpp"

class Obstacle : public Object {
  public:
    Obstacle();
    Obstacle(const string &obstacleName, int3 position, const string &name);

    void setObstacleName(const string &obstacleName);

    string getObstacleName();

    void printObject() const override;

  private:
    string obstacleName;
};
