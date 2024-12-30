#pragma once
#include "physics_engine/RigidBody.h"
#include "render_engine/RenderBody.h"
#include <memory>

struct Entity {
    std::unique_ptr<RigidBody> rigid_body;
    std::unique_ptr<RenderBody> render_body;

    Entity();
    Entity(std::unique_ptr<RigidBody>, std::unique_ptr<RenderBody>);
    ~Entity();
};
