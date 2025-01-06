#include "EntityComponentStorage.h"

#include "render_engine/shapes/Triangle.h"
#include "render_engine/shapes/Vertex.h"
#include <stdexcept>
#include <vector>

EntityComponentStorage::EntityComponentStorage() {
    size_t initial_capacity = 1024;
    colors.reserve(initial_capacity);
    rigid_bodies.reserve(initial_capacity);
}

EntityComponentStorage::~EntityComponentStorage() {}

EntityId EntityComponentStorage::add_entity(Entity &entity) {
    EntityId id = rigid_bodies.size();
    rigid_bodies.push_back(entity.rigid_body);
    colors.push_back(entity.render_body.color);

    return id;
}

RenderBody EntityComponentStorage::get_render_body(EntityId entity_id) {
    RenderBody body;
    body.position = rigid_bodies[entity_id].position;
    body.color = colors[entity_id];
    body.rotation = rigid_bodies[entity_id].rotation;
    body.shape = rigid_bodies[entity_id].shape_data;
    return body;
}

std::vector<Vertex> EntityComponentStorage::get_vertices(EntityId entity_id) {
    switch (rigid_bodies[entity_id].shape_data.shape) {
    case Shape::Triangle:
        return Triangle::vertices;
    case Shape::Square:
    case Shape::Circle:
        throw std::invalid_argument("Shape does is not yet implemented");
    default:
        return std::vector<Vertex>{};
    }
}
