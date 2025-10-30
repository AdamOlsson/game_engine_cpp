#include "image/Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb/stb_image.h"

#include <fstream>
#include <iostream>
#include <sstream>

image::Image::Image(image::ImageDimensions &dim, unsigned char *pixels, size_t size,
                    bool use_stbi_free = true)
    : dimensions(dim), pixels(pixels), size(size), m_use_stbi_free(use_stbi_free) {}

image::Image::~Image() {
    if (pixels == nullptr) {
        return;
    }

    if (m_use_stbi_free) {
        stbi_image_free(pixels);
    } else {
        delete[] pixels;
    }
    pixels = nullptr;
}

image::Image image::Image::load_rgba_image(const uint8_t *bytes, const size_t num_bytes) {
    return load_image(bytes, num_bytes, STBI_rgb_alpha);
}

image::Image image::Image::load_rgb_image(const uint8_t *bytes, const size_t num_bytes) {
    return load_image(bytes, num_bytes, STBI_rgb);
}

image::Image image::Image::load_grey_image(const uint8_t *bytes, const size_t num_bytes) {
    return load_image(bytes, num_bytes, STBI_grey);
}

image::Image image::Image::load_grey_alpha_image(const uint8_t *bytes,
                                                 const size_t num_bytes) {
    return load_image(bytes, num_bytes, STBI_grey_alpha);
}

image::Image image::Image::load_image(const uint8_t *bytes, const size_t num_bytes,
                                      int image_type) {
    int width;
    int height;
    int channels;

    stbi_uc *pixels =
        stbi_load_from_memory(reinterpret_cast<const unsigned char *>(bytes), num_bytes,
                              &width, &height, &channels, image_type);
    size_t size = width * height * 4;

    if (!pixels) {
        std::stringstream ss;
        ss << "Failed to load image bytes from memory" << std::endl;
        throw std::runtime_error(ss.str());
    }

    image::ImageDimensions dim = image::ImageDimensions{
        static_cast<unsigned int>(width), static_cast<unsigned int>(height),
        static_cast<unsigned int>(channels)};

    return image::Image(dim, pixels, size, true);
}

image::Image image::Image::empty() {
    image::ImageDimensions dim = image::ImageDimensions{1, 1, 4};
    unsigned char *pixels = new unsigned char[4];
    return image::Image(dim, pixels, 4, false);
}

std::vector<char> image::Image::read_file(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error(std::format("Failed to open file {}", filename));
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}
