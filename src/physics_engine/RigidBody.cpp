#include "physics_engine/RigidBody.h"
#include <array>

bool RigidBody::is_point_inside(glm::vec2 &point) {
    // Only handle triangle case
    if (shape_data.shape != Shape::Triangle) {
        return false;
    }

    // Get the triangle side length
    float side = shape_data.param.triangle.side;

    // First, translate the point relative to triangle's center
    glm::vec2 local_point = point - glm::vec2(position.x, position.y);

    // Rotate the point opposite to the triangle's rotation to align with local space
    float cos_rot = cos(-rotation);
    float sin_rot = sin(-rotation);
    glm::vec2 rotated_point =
        glm::vec2(local_point.x * cos_rot - local_point.y * sin_rot,
                  local_point.x * sin_rot + local_point.y * cos_rot);

    // For an equilateral triangle centered at origin:
    // height = side * sqrt(3) / 2
    // distance from center to any vertex = 2 * height / 3
    float height = side * sqrt(3.0f) / 2.0f;
    float center_to_vertex = 2.0f * height / 3.0f;

    // Calculate the three vertices of the triangle in local space
    std::array<glm::vec2, 3> vertices = {
        glm::vec2(0.0f, center_to_vertex),       // Top vertex
        glm::vec2(-side / 2.0f, -height / 3.0f), // Bottom left vertex
        glm::vec2(side / 2.0f, -height / 3.0f)   // Bottom right vertex
    };

    // Use barycentric coordinates to determine if point is inside
    float denominator =
        ((vertices[1].y - vertices[2].y) * (vertices[0].x - vertices[2].x) +
         (vertices[2].x - vertices[1].x) * (vertices[0].y - vertices[2].y));

    float a = ((vertices[1].y - vertices[2].y) * (rotated_point.x - vertices[2].x) +
               (vertices[2].x - vertices[1].x) * (rotated_point.y - vertices[2].y)) /
              denominator;
    float b = ((vertices[2].y - vertices[0].y) * (rotated_point.x - vertices[2].x) +
               (vertices[0].x - vertices[2].x) * (rotated_point.y - vertices[2].y)) /
              denominator;
    float c = 1.0f - a - b;

    // Point is inside if all barycentric coordinates are between 0 and 1
    return a >= 0.0f && a <= 1.0f && b >= 0.0f && b <= 1.0f && c >= 0.0f && c <= 1.0f;
}
