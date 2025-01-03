#pragma once
#include "glm/glm.hpp"

enum class EntityShape { None, Triangle, Square, Circle };

struct EntityBuilder {
    glm::vec3 position = glm::vec3(0.0, 0.0, 0.0);
    glm::vec3 color = glm::vec3(1.0, 0.0, 0.0);
    glm::float32 rotation = 0.0;
    EntityShape shape = EntityShape::None;
};
