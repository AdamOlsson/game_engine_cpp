#include "EntityComponentStorage.h"

#include "render_engine/shapes/Geometry.h"
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

void EntityComponentStorage::set_entity_position(EntityId entity_id, float x_pos,
                                                 float y_pos) {
    rigid_bodies[entity_id].position.x = x_pos;
    rigid_bodies[entity_id].position.y = y_pos;
}

RenderBody EntityComponentStorage::get_render_body(EntityId entity_id) {
    RenderBody body;
    body.position = rigid_bodies[entity_id].position;
    body.position.y *= 1.0;
    body.color = colors[entity_id];
    body.rotation = rigid_bodies[entity_id].rotation;
    body.shape = rigid_bodies[entity_id].shape;
    return body;
}

/*std::vector<Vertex> EntityComponentStorage::get_vertices(EntityId entity_id) {*/
/*    switch (rigid_bodies[entity_id].shape_data.shape) {*/
/*    case Shape::Triangle:*/
/*        return Geometry::triangle_vertices;*/
/*    case Shape::Rectangle:*/
/*    case Shape::Circle:*/
/*        throw std::invalid_argument("Shape does is not yet implemented");*/
/*    default:*/
/*        return std::vector<Vertex>{};*/
/*    }*/
/*}*/
