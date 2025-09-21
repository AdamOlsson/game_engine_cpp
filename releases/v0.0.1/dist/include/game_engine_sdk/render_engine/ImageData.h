#pragma once

#include <cstddef>
#include <cstdint>

struct ImageDimension {
    unsigned int width;
    unsigned int height;
    unsigned int channels;
};

class ImageData {
  private:
    bool m_use_stbi_free;

    ImageData(ImageDimension &dim, unsigned char *pixels, size_t size,
              bool use_stbi_free);

  public:
    ImageDimension dimension;
    unsigned char *pixels;
    size_t size;

    ImageData() = default;
    ImageData(ImageData &&other) noexcept = delete;
    ImageData(const ImageData &other) = delete;
    ImageData &operator=(ImageData &&other) noexcept = delete;
    ImageData &operator=(const ImageData &other) noexcept = delete;

    ~ImageData();
    static ImageData load_rgba_image(const uint8_t *bytes, const size_t num_bytes);
    static ImageData empty();
};
