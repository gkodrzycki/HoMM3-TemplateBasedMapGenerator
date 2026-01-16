#pragma once

#include "../global/int3.hpp"
#include "./Object.hpp"
#include "./ResourceType.hpp"

class Resource : public Object {
  public:
    Resource();
    Resource(const ResourceType &resourceType, int quantity, const int3 &position,
             const string &name);

    void setQuantity(int quantity);
    int getQuantity() const;

    void setResourceType(ResourceType resourceType);
    ResourceType getResourceType() const;

    void printObject() const override;

  private:
    int quantity;

    ResourceType resourceType;
};
