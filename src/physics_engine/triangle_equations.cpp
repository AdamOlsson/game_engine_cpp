#include "triangle_equations.h"
#include "equations/equations.h"
#include "io.h"

std::vector<glm::vec3> get_triangle_vertices(const RigidBody &body,
                                             const glm::vec3 &translate,
                                             const float rotate) {

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

std::vector<glm::vec3> get_triangle_edges(const RigidBody &body) {
    std::vector<glm::vec3> vertices = get_triangle_vertices(body);

    glm::vec3 left_edge = vertices[1] - vertices[0];
    glm::vec3 bottom_edge = vertices[2] - vertices[1];
    glm::vec3 right_edge = vertices[0] - vertices[2];

    std::vector<glm::vec3> edges = {left_edge, bottom_edge, right_edge};
    return edges;
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

bool is_point_inside_triangle(const RigidBody &body, const WorldPoint &point) {
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

WorldPoint closest_point_on_triangle(const RigidBody &body,
                                     const WorldPoint &other_point) {

    if (body.is_point_inside(other_point)) {
        return other_point;
    }

    auto local_point = other_point - body.position;
    Equations::rotate_z_mut(local_point, -body.rotation);

    glm::vec3 translate = -body.position;
    float rotate = -body.rotation;
    std::vector<glm::vec3> vertices = get_triangle_vertices(body, translate, rotate);
    /*std::cout << "local_point: " << local_point << std::endl;*/
    /*std::cout << "vertice 0: " << vertices[0] << std::endl;*/
    /*std::cout << "vertice 1: " << vertices[1] << std::endl;*/
    /*std::cout << "vertice 2: " << vertices[2] << std::endl;*/

    float min_dist = std::numeric_limits<float>::max();
    glm::vec3 closest_point;

    for (size_t i = 0; i < 3; ++i) {
        size_t j = (i + 1) % 3;

        glm::vec3 edge = vertices[j] - vertices[i];
        float edge_length_squared = glm::dot(edge, edge);

        glm::vec3 point_vec = local_point - vertices[i];
        float t = glm::dot(point_vec, edge) / edge_length_squared;
        t = std::max(0.0f, std::min(1.0f, t));

        glm::vec3 point_on_edge = vertices[i] + t * edge;

        float dist = Equations::length2(point_on_edge - local_point);
        if (dist < min_dist) {
            min_dist = dist;
            closest_point = point_on_edge;
        }
    }

    /*std::cout << "closest_point local space: " << closest_point << std::endl;*/
    Equations::rotate_z_mut(closest_point, body.rotation);
    closest_point += static_cast<glm::vec3>(body.position);

    /*std::cout << "closest_point global space: " << closest_point << std::endl;*/

    return static_cast<WorldPoint>(closest_point);

    return WorldPoint(0.0f, 0.0f, 0.0f);
}
