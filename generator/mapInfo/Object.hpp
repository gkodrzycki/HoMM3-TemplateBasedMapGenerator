#pragma once

#include "../global/Global.hpp"
#include "../global/int3.hpp"

class Object {
  public:
    virtual ~Object() = default;

    Object();
    Object(int3 position, string name);
    Object(int3 position, string name, int3 size);
    Object(int3 position, string name, int3 size, int3 entryPoint, vector<string> realSize);

    int3 getPosition() const;
    string getName() const;
    int3 getSize() const;
    int3 getEntryPoint() const;
    vector<string> &getRealSize();

    void setPosition(int3 position);
    void setName(string name);
    void setSize(int3 size);
    void setRealSize(vector<string> realSize);
    void setEntryPoint(int3 entryPoint);

    virtual void printObject() const;

  private:
    int3 position;
    int3 size;
    int3 entryPoint;
    string name;
    vector<string> realSize;
};
