#pragma once

#include "../global/int3.hpp"
#include "./ArtifactType.hpp"
#include "./Treasure.hpp"

class Artifact : public Treasure {
  public:
    Artifact();
    Artifact(const ArtifactType &artifactType, const int3 &position, const string &name);

    void setArtifactType(ArtifactType artifactType);
    ArtifactType getArtifactType() const;

    void printObject() const override;

  private:
    ArtifactType artifactType;
};
