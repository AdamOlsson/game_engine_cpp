#pragma once

#include "EntityBuilder.h"
#include "glm/fwd.hpp"
#include "glm/glm.hpp"
#include "physics_engine/RigidBody.h"
#include "render_engine/RenderBody.h"
#include "render_engine/shapes/Vertex.h"
#include <vector>

using EntityId = size_t;

class EntityComponentStorage {
  public:
    EntityComponentStorage();
    ~EntityComponentStorage();

    size_t add_entity(EntityBuilder &entity);
    RenderBody get_render_body(EntityId);
    RigidBody get_rigid_body(EntityId);
    std::vector<Vertex> get_vertices(EntityId);

    void update_rotation(EntityId, glm::float32 new_value);

  private:
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> colors;
    std::vector<glm::float32> rotations;
    std::vector<EntityShape> shapes;
};
