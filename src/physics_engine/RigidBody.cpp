#include "physics_engine/RigidBody.h"
#include "glm/fwd.hpp"

RigidBody::RigidBody() {};

RigidBody::RigidBody(glm::vec3 position, glm::float32_t rotation)
    : position(position), rotation(rotation) {};

RigidBody::~RigidBody() {};
