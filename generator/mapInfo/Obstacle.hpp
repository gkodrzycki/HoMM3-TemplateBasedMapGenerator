#pragma once

#include "../global/int3.hpp"
#include "./Object.hpp"

class Obstacle : public Object {
  public:
    Obstacle();
    Obstacle(const string &obstacleName, const int3 &position, const string &name);
    Obstacle(const string &obstacleName, const int3 &position, const string &name, const int3 &size,
             const int3 &entryPoint, const vector<string> &realSize);

    void setObstacleName(const string &obstacleName);
    void setExclusionGroup(const string &group);
    void setMinSpacing(int spacing);

    const string getObstacleName() const;
    const string &getExclusionGroup() const;
    int getMinSpacing() const;

    void printObject() const override;

  private:
    string obstacleName;
    string exclusionGroup;
    int minSpacing = 0;
};
