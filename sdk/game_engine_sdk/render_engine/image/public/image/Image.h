#pragma once

#include <cstddef>
#include <cstdint>

namespace image {
struct ImageDimensions {
    unsigned int width;
    unsigned int height;
    unsigned int channels;
};

class Image {
  private:
    bool m_use_stbi_free;

    Image(ImageDimensions &dim, unsigned char *pixels, size_t size, bool use_stbi_free);

  public:
    ImageDimensions dimensions;
    unsigned char *pixels;
    size_t size;

    Image() = default;
    Image(Image &&other) noexcept = delete;
    Image(const Image &other) = delete;
    Image &operator=(Image &&other) noexcept = delete;
    Image &operator=(const Image &other) noexcept = delete;

    ~Image();
    static Image load_rgba_image(const uint8_t *bytes, const size_t num_bytes);
    static Image empty();
};
} // namespace image
