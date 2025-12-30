#pragma once

#include "../global/int3.hpp"
#include "./Object.hpp"

class Road : public Object {
  public:
    Road();
    Road(const int &roadTier, const int3 &position, const string &name);

    void setRoadTier(int roadTier);

    int getRoadTier() const;

    void printObject() const override;

  private:
    int roadTier;
};
