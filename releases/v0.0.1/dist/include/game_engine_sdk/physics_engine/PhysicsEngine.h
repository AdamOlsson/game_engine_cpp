#pragma once

#include "game_engine_sdk/physics_engine/RigidBody.h"
#include <vector>
class PhysicsEngine {
  public:
    PhysicsEngine();
    ~PhysicsEngine();

    void update();

  private:
    std::vector<RigidBody> bodies;
};
