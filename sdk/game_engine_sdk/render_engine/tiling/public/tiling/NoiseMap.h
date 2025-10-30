#pragma once
#include "image/Image.h"
#include "logger/logger.h"
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
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
    };
    // clang-format on

    NoiseMap() = default;
    NoiseMap(std::vector<float> &&noise, const size_t width, const size_t height)
        : noise(std::move(noise)), width(width), height(height) {}
    ~NoiseMap() = default;

    static std::unique_ptr<NoiseMap> unique_from_filepath(const std::string filename) {
        auto bytes = image::Image::read_file(filename);
        const auto image_data = image::Image::load_grey_image(
            reinterpret_cast<uint8_t *>(bytes.data()), bytes.size());
        const auto image_dims = image_data.dimensions;

        /*logger::debug("sizeof(unsigned char): ", sizeof(unsigned char));*/
        /*logger::debug("sizeof(uint8_t): ", sizeof(uint8_t));*/
        /*logger::debug("Image Dimensions: ", image_dims);*/

        if (image_dims.channels != 1) {
            throw std::runtime_error(
                std::format("Expected only 1 channel in the image but found {}",
                            image_dims.channels));
        }

        // Image class returns pixels in char format (0 to 255) so we make floats ranging
        // from 0.0 to 1.0
        std::vector<float> noise;
        const size_t noise_map_size = image_dims.width * image_dims.height;
        noise.reserve(noise_map_size);
        for (auto i = 0; i < noise_map_size; i++) {
            noise.push_back(static_cast<float>(image_data.pixels[i]) / 255.0f);
            /*logger::debug(std::format("{}: {}", i, noise.back()));*/
        }

        return std::make_unique<NoiseMap>(std::move(noise), image_dims.width,
                                          image_dims.height);
    }

    size_t size() const { return noise.size(); }
};

} // namespace tiling
