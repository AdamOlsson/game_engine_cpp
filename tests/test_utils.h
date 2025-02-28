#pragma once

#include "glm/glm.hpp"
#include "io.h"
#include <gtest/gtest.h>

constexpr float MAX_DIFF = 1e-3;

void expect_near(const glm::vec3 &expected, const glm::vec3 &v, const float epsilon);
