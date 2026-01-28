#pragma once

#include "../global/int3.hpp"
#include "./ResourceType.hpp"
#include "./Treasure.hpp"

class Resource : public Treasure {
  public:
    Resource();
    Resource(const ResourceType &resourceType, int quantity, const int3 &position,
             const string &name);

    void setResourceType(ResourceType resourceType);
    ResourceType getResourceType() const;

    void printObject() const override;

  private:
    ResourceType resourceType;
};
