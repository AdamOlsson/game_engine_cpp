#include "physics_engine/RigidBody.h"
#include "Shape.h"
#include "equations/equations.h"

std::vector<glm::vec3>
get_rectangle_vertices(const RigidBody &body,
                       const glm::vec3 &translate = glm::vec3(0.0, 0.0, 0.0),
                       const float rotate = 0.0) {
    auto rect = body.shape.get<Rectangle>();
    float width = rect.width;
    float height = rect.height;

    glm::vec3 top_left = glm::vec3(-width / 2.0, height / 2.0, 0.0);
    glm::vec3 bot_left = glm::vec3(-width / 2.0, -height / 2.0, 0.0);
    glm::vec3 bot_right = glm::vec3(width / 2.0, -height / 2.0, 0.0);
    glm::vec3 top_right = glm::vec3(width / 2.0, height / 2.0, 0.0);

    float rotation = body.rotation + rotate;
    Equations::rotate_z_mut(top_left, rotation);
    Equations::rotate_z_mut(bot_left, rotation);
    Equations::rotate_z_mut(bot_right, rotation);
    Equations::rotate_z_mut(top_right, rotation);

    glm::vec3 translation = body.position + translate;
    top_left += translation;
    bot_left += translation;
    bot_right += translation;
    top_right += translation;

    std::vector<glm::vec3> vertices = {top_left, bot_left, bot_right, top_right};
    return vertices;
}

std::vector<glm::vec3>
get_triangle_vertices(const RigidBody &body,
                      const glm::vec3 &translate = glm::vec3(0.0, 0.0, 0.0),
                      const float rotate = 0.0) {

    auto triangle = body.shape.get<Triangle>();
    float side = triangle.side;
    float height = side * std::sqrt(3.0f) / 2.0f;
    float center_to_vertex = 2.0f * height / 3.0f;

    glm::vec3 v1 = glm::vec3(0.0f, center_to_vertex, 0.0);
    glm::vec3 v2 = glm::vec3(-side / 2.0f, -height / 3.0f, 0.0);
    glm::vec3 v3 = glm::vec3(side / 2.0f, -height / 3.0f, 0.0);

    float rotation = body.rotation + rotate;
    Equations::rotate_z_mut(v1, rotation);
    Equations::rotate_z_mut(v2, rotation);
    Equations::rotate_z_mut(v3, rotation);

    glm::vec3 translation = body.position + translate;

    v1 += translation;
    v2 += translation;
    v3 += translation;

    std::vector<glm::vec3> vertices = {v1, v2, v3};

    /*std::cout << "vertice 0: " << vertices[0] << std::endl;*/
    /*std::cout << "vertice 1: " << vertices[1] << std::endl;*/
    /*std::cout << "vertice 2: " << vertices[2] << std::endl;*/

    return vertices;
}

std::vector<glm::vec3> get_rectangle_edges(const RigidBody &body) {
    std::vector<glm::vec3> vertices = get_rectangle_vertices(body);

    /*std::cout << "vertice 1: " << vertices[0] << std::endl;*/
    /*std::cout << "vertice 2: " << vertices[1] << std::endl;*/
    /*std::cout << "vertice 2: " << vertices[2] << std::endl;*/
    /*std::cout << "vertice 3: " << vertices[3] << std::endl;*/

    glm::vec3 left_edge = vertices[1] - vertices[0];
    glm::vec3 bottom_edge = vertices[2] - vertices[1];
    glm::vec3 right_edge = vertices[3] - vertices[2];
    glm::vec3 top_edge = vertices[0] - vertices[3];

    std::vector<glm::vec3> normals = {left_edge, bottom_edge, right_edge, top_edge};
    return normals;
}

std::vector<glm::vec3> get_triangle_edges(const RigidBody &body) {
    std::vector<glm::vec3> vertices = get_triangle_vertices(body);

    glm::vec3 left_edge = vertices[1] - vertices[0];
    glm::vec3 bottom_edge = vertices[2] - vertices[1];
    glm::vec3 right_edge = vertices[0] - vertices[2];

    std::vector<glm::vec3> edges = {left_edge, bottom_edge, right_edge};
    return edges;
}

std::vector<glm::vec3> get_rectangle_normals(const RigidBody &body) {
    auto edges = get_rectangle_edges(body);

    /*std::cout << "edge 1: " << edges[0] << std::endl;*/
    /*std::cout << "edge 2: " << edges[1] << std::endl;*/
    /*std::cout << "edge 2: " << edges[2] << std::endl;*/
    /*std::cout << "edge 3: " << edges[3] << std::endl;*/

    auto left_normal = glm::normalize(edges[0]);
    auto bottom_normal = glm::normalize(edges[1]);
    auto right_normal = glm::normalize(edges[2]);
    auto top_normal = glm::normalize(edges[3]);

    Equations::clockwise_perp_z_mut(left_normal);
    Equations::clockwise_perp_z_mut(bottom_normal);
    Equations::clockwise_perp_z_mut(right_normal);
    Equations::clockwise_perp_z_mut(top_normal);

    std::vector<glm::vec3> normals = {left_normal, bottom_normal, right_normal,
                                      top_normal};
    /*std::cout << "normal 1: " << normals[0] << std::endl;*/
    /*std::cout << "normal 2: " << normals[1] << std::endl;*/
    /*std::cout << "normal 2: " << normals[2] << std::endl;*/
    /*std::cout << "normal 3: " << normals[3] << std::endl;*/

    return normals;
}

std::vector<glm::vec3> get_triangle_normals(const RigidBody &body) {
    auto edges = get_triangle_edges(body);

    auto left_normal = glm::normalize(edges[0]);
    auto bottom_normal = glm::normalize(edges[1]);
    auto right_normal = glm::normalize(edges[2]);

    Equations::clockwise_perp_z_mut(left_normal);
    Equations::clockwise_perp_z_mut(bottom_normal);
    Equations::clockwise_perp_z_mut(right_normal);

    std::vector<glm::vec3> normals = {left_normal, bottom_normal, right_normal};
    return normals;
}

std::vector<glm::vec3> RigidBody::edges() const {
    return std::visit(
        [this](auto &&arg) -> std::vector<glm::vec3> {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Triangle>) {
                return get_triangle_edges(*this);
            } else if constexpr (std::is_same_v<T, Rectangle>) {
                return get_rectangle_edges(*this);
            } else {
                throw std::runtime_error("Shape not implemented (edges())");
            }
        },
        shape.params);
}

std::vector<glm::vec3> RigidBody::vertices() const {
    return std::visit(
        [this](auto &&arg) -> std::vector<glm::vec3> {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Triangle>) {
                return get_triangle_vertices(*this);
            } else if constexpr (std::is_same_v<T, Rectangle>) {
                return get_rectangle_vertices(*this);
            } else {
                throw std::runtime_error("Shape not implemented (vertices())");
            }
        },
        shape.params);
}

std::vector<glm::vec3> RigidBody::normals() const {
    return std::visit(
        [this](auto &&arg) -> std::vector<glm::vec3> {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Triangle>) {
                return get_triangle_normals(*this);
            } else if constexpr (std::is_same_v<T, Rectangle>) {
                return get_rectangle_normals(*this);
            } else {
                throw std::runtime_error("Shape not implemented (normals())");
            }
        },
        shape.params);
}

bool is_point_inside_triangle(const RigidBody &body, WorldPoint &point) {
    glm::vec3 local_point = point - body.position;

    Equations::rotate_z_mut(local_point, -body.rotation);

    glm::vec3 translate = -body.position;
    float rotate = -body.rotation;

    std::vector<glm::vec3> vertices = get_triangle_vertices(body, translate, rotate);

    glm::vec3 v0v1 = vertices[1] - vertices[0];
    glm::vec3 v0v2 = vertices[2] - vertices[0];
    glm::vec3 v0p = local_point - vertices[0];

    float dot00 = glm::dot(v0v1, v0v1);
    float dot01 = glm::dot(v0v1, v0v2);
    float dot02 = glm::dot(v0v1, v0p);
    float dot11 = glm::dot(v0v2, v0v2);
    float dot12 = glm::dot(v0v2, v0p);

    float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    return (u >= 0.0f) && (v >= 0.0f) && (u + v <= 1.0f);
}

bool is_point_inside_rectangle(const RigidBody &body, WorldPoint &point) {
    // TODO: Use get_rectangle_vertices as with triangle variant
    glm::vec2 local_point =
        glm::vec2(point.x, point.y) - glm::vec2(body.position.x, body.position.y);
    Equations::rotate_mut(local_point, body.rotation);
    auto rect = body.shape.get<Rectangle>();
    float half_width = rect.width / 2.0;
    float half_height = rect.height / 2.0;

    return std::abs(local_point.x) <= half_width &&
           std::abs(local_point.y) <= half_height;
};

bool RigidBody::is_point_inside(WorldPoint &point) const {
    return std::visit(
        [this, &point](auto &&arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Triangle>) {
                return is_point_inside_triangle(*this, point);
            } else if constexpr (std::is_same_v<T, Rectangle>) {
                return is_point_inside_rectangle(*this, point);
            } else {
                throw std::runtime_error("Unsupported shape type");
            }
        },
        shape.params);
}
