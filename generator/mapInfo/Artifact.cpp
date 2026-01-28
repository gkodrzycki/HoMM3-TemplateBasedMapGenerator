#include "./Artifact.hpp"

Artifact::Artifact() : Treasure(0, int3(1, 1, 1), "Artifact") {
    artifactType = ArtifactType::ARTIFACT_UNKNOWN;
};

Artifact::Artifact(const ArtifactType &artifactType, const int3 &position, const string &name)
    : Treasure(1, position, name) {
    this->artifactType = artifactType;
}

void Artifact::setArtifactType(ArtifactType artifactType) { this->artifactType = artifactType; }
ArtifactType Artifact::getArtifactType() const { return artifactType; }

void Artifact::printObject() const {
    cerr << "Artifact '" << getEnumName<ArtifactType>(artifactType) << "' x at "
         << getPosition().toString() << "\n";
}
