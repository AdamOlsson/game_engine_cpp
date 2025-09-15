#pragma once

#include "game_engine_sdk/physics_engine/RigidBody.h"

float get_circle_bounding_volume_radius(const RigidBody &body);
bool is_point_inside_circle(const RigidBody &body, const WorldPoint &point);
WorldPoint closest_point_on_circle(const RigidBody &body, const WorldPoint &point);
float circle_inertia(const RigidBody &body);
