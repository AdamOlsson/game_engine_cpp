#include "rectangle_equations.h"
#include "Coordinates.h"
#include "equations/equations.h"
#include "io.h"
#include "physics_engine/RigidBody.h"

std::vector<glm::vec3> get_rectangle_vertices(const RigidBody &body,
                                              const glm::vec3 &translate,
                                              const float rotate) {
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
    return normals;
}

float get_rectangle_bounding_volume_radius(const RigidBody &body) {
    auto rect = body.shape.get<Rectangle>();
    float half_width = rect.width / 2.0;
    float half_height = rect.height / 2.0;
    return std::sqrt(half_width * half_width + half_height * half_height);
}

bool is_point_inside_rectangle(const RigidBody &body, const WorldPoint &point) {
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

WorldPoint closest_point_on_rectangle(const RigidBody &body,
                                      const WorldPoint &other_point) {

    auto rectangle = body.shape.get<Rectangle>();
    float width = rectangle.width;
    float height = rectangle.height;

    auto local_circle_center = other_point - body.position;
    Equations::rotate_z_mut(local_circle_center, -body.rotation);

    float local_closest_point_on_rect_x =
        std::max(-width / 2.0f, std::min(local_circle_center.x, width / 2.0f));
    float local_closest_point_on_rect_y =
        std::max(-height / 2.0f, std::min(local_circle_center.y, height / 2.0f));

    glm::vec3 local_closest_point_on_rect(local_closest_point_on_rect_x,
                                          local_closest_point_on_rect_y, 0.0f);

    Equations::rotate_z_mut(local_closest_point_on_rect, body.rotation);
    local_closest_point_on_rect += static_cast<glm::vec3>(body.position);

    return static_cast<WorldPoint>(local_closest_point_on_rect);
}

float rectangle_inertia(const RigidBody &body) {
    auto rectangle = body.shape.get<Rectangle>();
    return (rectangle.width * rectangle.width + rectangle.height * rectangle.height) *
           body.mass / 12.0f;
}
