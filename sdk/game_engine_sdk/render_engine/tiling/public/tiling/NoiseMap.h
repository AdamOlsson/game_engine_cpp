#pragma once
#include <vector>

namespace tiling {
class NoiseMap {
  private:
  public:
    unsigned int width = 5;
    unsigned int height = 5;

    // clang-format off
    std::vector<float> noise = {
        0.0, 0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
    };
    // clang-format on

    NoiseMap() = default;
    NoiseMap(std::vector<float> &&noise, const size_t width, const size_t height)
        : noise(std::move(noise)), width(width), height(height) {}
    ~NoiseMap() = default;

    static NoiseMap unique_from_filepath();

    size_t size() const { return noise.size(); }
};

} // namespace tiling
