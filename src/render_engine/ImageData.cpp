#include "ImageData.h"

#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb/stb_image.h"

#include <iostream>
#include <sstream>

ImageData::ImageData(ImageDimension &dim, unsigned char *pixels, size_t size)
    : dimension(dim), pixels(pixels), size(size) {}

ImageData::~ImageData() { stbi_image_free(pixels); }

ImageData ImageData::load_rgba_image(const uint8_t *bytes, const size_t num_bytes) {
    int width;
    int height;
    int channels;

    stbi_uc *pixels =
        stbi_load_from_memory(reinterpret_cast<const unsigned char *>(bytes), num_bytes,
                              &width, &height, &channels, STBI_rgb_alpha);
    size_t size = width * height * 4;

    if (!pixels) {
        std::stringstream ss;
        ss << "Failed to load image bytes from memory" << std::endl;
        throw std::runtime_error(ss.str());
    }

    ImageDimension dim = ImageDimension{static_cast<unsigned int>(width),
                                        static_cast<unsigned int>(height),
                                        static_cast<unsigned int>(channels)};

    return ImageData(dim, pixels, size);
}
