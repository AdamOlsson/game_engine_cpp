#pragma once

#include "glm/fwd.hpp"
#include "physics_engine/RigidBody.h"
#include "physics_engine/collision_resolver.h"
#include "render_engine/RenderBody.h"

constexpr glm::vec3 RED{0.7, 0.0, 0.0};
constexpr glm::vec3 GREEN{0.0, 0.7, 0.0};
constexpr glm::vec3 BLUE{0.0, 0.0, 0.7};
constexpr glm::vec3 YELLOW{0.7, 0.7, 0.0};
constexpr glm::vec3 CIAN{0.0, 0.7, 0.7};
const std::vector<glm::vec3> COLORS{RED, GREEN, BLUE, YELLOW, CIAN};

// TODO: Make the RigidBodyBuilder be able to produce constexpr (low priority)
const RigidBody BOTTOM_BORDER = RigidBodyBuilder()
                                    .position(WorldPoint(0.0, -450.0f, 0.0f))
                                    .mass(FLT_MAX)
                                    .shape(Shape::create_rectangle_data(900.0f, 100.0f))
                                    .build();
const RigidBody RIGHT_BORDER = RigidBodyBuilder()
                                   .position(WorldPoint(450.0, 0.0f, 0.0f))
                                   .mass(FLT_MAX)
                                   .shape(Shape::create_rectangle_data(100.0f, 900.0f))
                                   .build();

const RigidBody TOP_BORDER = RigidBodyBuilder()
                                 .position(WorldPoint(0.0, 450.0f, 0.0f))
                                 .mass(FLT_MAX)
                                 .shape(Shape::create_rectangle_data(900.0f, 100.0f))
                                 .build();

const RigidBody LEFT_BORDER = RigidBodyBuilder()
                                  .position(WorldPoint(-450.0, 0.0f, 0.0f))
                                  .mass(FLT_MAX)
                                  .shape(Shape::create_rectangle_data(100.0f, 900.0f))
                                  .build();

void apply_physics(const float dt, RigidBody &body);
void constrain_to_window(const float dt, CollisionSolver &solver, RigidBody &body);
void check_collision_with_spinner(const float dt, CollisionSolver &solver,
                                  const RigidBody &spinner, RigidBody &body);
void apply_correction(const float dt, const Correction &correction, RigidBody &body);
