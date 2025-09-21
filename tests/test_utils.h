#pragma once

#include "game_engine_sdk/io.h"
#include "glm/glm.hpp"
#include <gtest/gtest.h>

constexpr float MAX_DIFF = 1e-3;

void expect_near(const glm::vec3 &expected, const glm::vec3 &v, const float epsilon);
