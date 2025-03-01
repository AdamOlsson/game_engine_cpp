#include "test_utils.h"

void expect_near(const glm::vec3 &expected, const glm::vec3 &v, const float epsilon) {
    std::cout << "Expected " << expected << " found " << v << std::endl;
    EXPECT_NEAR(expected.x, v.x, epsilon);
    EXPECT_NEAR(expected.y, v.y, epsilon);
    EXPECT_NEAR(expected.z, v.z, epsilon);
}
