#pragma once

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <vector>

namespace image {
struct ImageDimensions {
    unsigned int width;
    unsigned int height;
    unsigned int channels;

    friend std::ostream &operator<<(std::ostream &os, const ImageDimensions &obj) {
        return os << "ImageDimensions(" << obj.width << ", " << obj.height << ", "
                  << obj.channels << ")";
    }
};

class Image {
  private:
    bool m_use_stbi_free;

    Image(ImageDimensions &dim, unsigned char *pixels, size_t size, bool use_stbi_free);

    static image::Image load_image(const uint8_t *bytes, const size_t num_bytes,
                                   int image_type);

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
    static Image load_rgb_image(const uint8_t *bytes, const size_t num_bytes);
    static Image load_grey_image(const uint8_t *bytes, const size_t num_bytes);
    static Image load_grey_alpha_image(const uint8_t *bytes, const size_t num_bytes);

    static Image empty();
    static std::vector<char> read_file(const std::string &filename);
};
} // namespace image
