#pragma once

#include "glm/glm.hpp"
#include "physics_engine/RigidBody.h"
#include "render_engine/RenderBody.h"
#include "render_engine/shapes/Vertex.h"
#include <vector>

using EntityId = size_t;

struct Entity {
    RigidBody rigid_body;
    RenderBody render_body;
};

class EntityComponentStorage {
  public:
    EntityComponentStorage();
    ~EntityComponentStorage();

    EntityId add_entity(Entity &);
    void set_entity_position(EntityId, float, float);
    RenderBody get_render_body(EntityId);
    RigidBody get_rigid_body(EntityId);
    /*std::vector<Vertex> get_vertices(EntityId);*/

    void update_rotation(EntityId, glm::float32 new_value);

    std::vector<RigidBody> rigid_bodies;
    std::vector<glm::vec3> colors;
};
