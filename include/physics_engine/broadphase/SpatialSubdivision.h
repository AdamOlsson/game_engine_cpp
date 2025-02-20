#pragma once

#include "physics_engine/RigidBody.h"
class SpatialSubdivision {

  private:
  public:
    SpatialSubdivision() = default;
    ~SpatialSubdivision() = default;

    // TODO: Return type TBD
    void collision_detection(const std::vector<const RigidBody> &bodies);
};
