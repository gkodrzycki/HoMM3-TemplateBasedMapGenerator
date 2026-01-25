#pragma once

#include "../global/int3.hpp"
#include "./Object.hpp"

class Road : public Object {
  public:
    Road();
    Road(const int &roadTier, const vector<int3> &path, const string &name);

    void setRoadTier(int roadTier);
    void setPath(const vector<int3> &path);

    int getRoadTier() const;
    vector<int3> getPath() const;

    void printObject() const override;

  private:
    int roadTier;
    vector<int3> path;
};
