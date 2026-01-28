#include "./Resource.hpp"

Resource::Resource() : Treasure(0, int3(1, 1, 1), "Resource") {
    resourceType = ResourceType::RESOURCE_UNKNOWN;
};

Resource::Resource(const ResourceType &resourceType, int quantity, const int3 &position,
                   const string &name)
    : Treasure(quantity, position, name) {
    this->resourceType = resourceType;
}

void Resource::setResourceType(ResourceType resourceType) { this->resourceType = resourceType; }
ResourceType Resource::getResourceType() const { return resourceType; }

void Resource::printObject() const {
    cerr << "Resource '" << getEnumName<ResourceType>(resourceType) << "' x" << getQuantity()
         << " at " << getPosition().toString() << "\n";
}
