#include "game_engine_sdk/equations/round.h"

float Round::round(float f) { return std::round(f * 1000.0f) / 1000.0f; }

Projection Round::round(const Projection &proj) {
    return Projection{.min = round(proj.min), .max = round(proj.max)};
}

Overlap Round::round(const Overlap &o) { return Overlap{.distance = round(o.distance)}; }

glm::vec3 Round::round(const glm::vec3 &vec) {
    return glm::vec3(std::round(vec.x * 1000.0f) / 1000.0f,
                     std::round(vec.y * 1000.0f) / 1000.0f,
                     std::round(vec.z * 1000.0f) / 1000.0f);
}

glm::vec2 Round::round(const glm::vec2 &vec) {
    return glm::vec2(std::round(vec.x * 1000.0f) / 1000.0f,
                     std::round(vec.y * 1000.0f) / 1000.0f);
}

void Round::round_mut(glm::vec3 &vec) {
    vec.x = std::round(vec.x * 1000.0f) / 1000.0f;
    vec.y = std::round(vec.y * 1000.0f) / 1000.0f;
    vec.z = std::round(vec.z * 1000.0f) / 1000.0f;
}

void Round::round_mut(glm::vec2 &vec) {
    vec.x = std::round(vec.x * 1000.0f) / 1000.0f;
    vec.y = std::round(vec.y * 1000.0f) / 1000.0f;
}

void Round::round_mut(Projection &proj) {
    proj.min = std::round(proj.min * 1000.0f) / 1000.0f;
    proj.max = std::round(proj.max * 1000.0f) / 1000.0f;
}

void Round::round_mut(Overlap &o) {
    o.distance = std::round(o.distance * 1000.0f) / 1000.0f;
}
