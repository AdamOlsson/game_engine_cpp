#pragma once

#include "game_engine_sdk/equations/projection.h"

struct Round {
    static float round(float);
    static Projection round(const Projection &);
    static Overlap round(const Overlap &);
    static glm::vec3 round(const glm::vec3 &);
    static glm::vec2 round(const glm::vec2 &);

    static void round_mut(Projection &);
    static void round_mut(Overlap &);
    static void round_mut(glm::vec3 &);
    static void round_mut(glm::vec2 &);
};
