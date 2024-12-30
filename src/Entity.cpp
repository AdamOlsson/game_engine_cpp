#include "Entity.h"
#include "physics_engine/RigidBody.h"
#include "render_engine/RenderBody.h"

Entity::Entity()
    : rigid_body(std::make_unique<RigidBody>()),
      render_body(std::make_unique<RenderBody>()) {}

Entity::Entity(std::unique_ptr<RigidBody> rigid_body,
               std::unique_ptr<RenderBody> render_body)
    : rigid_body(std::move(rigid_body)), render_body(std::move(render_body)) {}

Entity::~Entity() {}
