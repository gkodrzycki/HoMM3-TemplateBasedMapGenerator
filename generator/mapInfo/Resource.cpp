#include "./Resource.hpp"

Resource::Resource() : Object(int3(1, 1, 1), "Resource") {
    quantity     = 0;
    resourceType = ResourceType::RESOURCE_UNKNOWN;
};

Resource::Resource(const ResourceType &resourceType, int quantity, const int3 &position,
                   const string &name)
    : Object(position, name) {
    this->quantity     = quantity;
    this->resourceType = resourceType;
}

void Resource::setQuantity(int quantity) { this->quantity = quantity; }
int Resource::getQuantity() const { return quantity; }

void Resource::setResourceType(ResourceType resourceType) { this->resourceType = resourceType; }
ResourceType Resource::getResourceType() const { return resourceType; }

void Resource::printObject() const {
    cerr << "Resource '" << getEnumName<ResourceType>(resourceType) << "' x" << quantity << " at "
         << getPosition().toString() << "\n";
}
