#pragma once

#include "game_engine_sdk/physics_engine/RigidBody.h"
#include "game_engine_sdk/physics_engine/collision_resolver.h"
#include "game_engine_sdk/render_engine/colors.h"
#include "glm/fwd.hpp"

const std::vector<glm::vec4> COLORS{util::colors::RED, util::colors::GREEN,
                                    util::colors::BLUE, util::colors::YELLOW,
                                    util::colors::CYAN};

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
