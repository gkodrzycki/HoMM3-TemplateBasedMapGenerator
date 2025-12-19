#pragma once

#include "../global/Global.hpp"
#include "../global/int3.hpp"

class Object {
  public:
    virtual ~Object() = default;

    Object();
    Object(int3 position, string name);
    Object(int3 position, string name, int3 size);

    int3 getPosition() const;
    string getName() const;
    int3 getSize() const;

    void setPosition(int3 position);
    void setName(string name);
    void setSize(int3 size);

    virtual void printObject() const;

  private:
    int3 position;
    int3 size;
    string name;
};
