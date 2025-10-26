#pragma once
#include <vector>

namespace tiling {
class NoiseMap {
  private:
  public:
    unsigned int width = 4;
    unsigned int height = 3;

    // clang-format off
    std::vector<float> noise = {
        0.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 0.0,
    };
    // clang-format on

    /*NoiseMap() = default;*/
    ~NoiseMap() = default;

    size_t size() const { return noise.size(); }
};

} // namespace tiling
