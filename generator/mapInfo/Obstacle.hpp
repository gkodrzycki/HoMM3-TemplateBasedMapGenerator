#pragma once

#include "../global/int3.hpp"
#include "./Object.hpp"

class Obstacle : public Object {
  public:
    Obstacle();
    Obstacle(const string &obstacleName, const int3 &position, const string &name);

    void setObstacleName(const string &obstacleName);

    const string getObstacleName() const;

    void printObject() const override;

  private:
    string obstacleName;
};
