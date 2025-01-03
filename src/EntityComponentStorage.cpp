#include "EntityComponentStorage.h"

#include "physics_engine/RigidBody.h"
#include "render_engine/shapes/Triangle.h"
#include "render_engine/shapes/Vertex.h"
#include <stdexcept>
#include <vector>

EntityComponentStorage::EntityComponentStorage() {
    size_t initial_capacity = 1024;
    positions.reserve(initial_capacity);
    colors.reserve(initial_capacity);
    rotations.reserve(initial_capacity);
    shapes.reserve(initial_capacity);
}

EntityComponentStorage::~EntityComponentStorage() {}

EntityId EntityComponentStorage::add_entity(EntityBuilder &entity) {
    size_t id = positions.size();

    positions.push_back(entity.position);
    colors.push_back(entity.color);
    rotations.push_back(entity.rotation);
    shapes.push_back(entity.shape);

    return id;
}

RigidBody EntityComponentStorage::get_rigid_body(EntityId entity_id) {
    RigidBody body;
    body.position = &positions[entity_id];
    body.rotation = &rotations[entity_id];
    return body;
}

RenderBody EntityComponentStorage::get_render_body(EntityId entity_id) {
    RenderBody body;
    body.position = &positions[entity_id];
    body.color = &colors[entity_id];
    body.rotation = &rotations[entity_id];
    return body;
}

void EntityComponentStorage::update_rotation(EntityId entity_id, glm::float32 new_value) {
    rotations[entity_id] = new_value;
}

std::vector<Vertex> EntityComponentStorage::get_vertices(EntityId entity_id) {
    switch (shapes[entity_id]) {
    case EntityShape::Triangle:
        return Triangle::vertices;
    case EntityShape::Square:
    case EntityShape::Circle:
        throw std::invalid_argument("Shape does is not yet implemented");
    default:
        return std::vector<Vertex>{};
    }
}
