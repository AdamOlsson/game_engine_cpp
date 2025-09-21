#pragma once

#include "game_engine_sdk/render_engine/resources/Resource.h"
#include <cstdint>
#include <memory>
#include <vector>

class ImageResource : public Resource {

  private:
    // All resources
    const std::string name_;
    const unsigned int length_;
    const uint8_t *bytes_;

    ImageResource(const std::string &&name, const unsigned int length,
                  const uint8_t *bytes, const uint32_t width_px, const uint32_t height_px,
                  const uint8_t num_channels);

    friend class ImageResourceBuilder;

  public:
    // Image specific
    const uint32_t width_px;
    const uint32_t height_px;
    const uint8_t num_channels;

    const uint8_t *bytes() const override;
    const std::string &name() const override;
    const unsigned int length() const override;
};

std::vector<std::unique_ptr<ImageResource>> fetch_all_images();

class ImageResourceBuilder {
  private:
    // All resources
    std::string name_;
    unsigned int length_ = 0;
    unsigned char *bytes_ = nullptr;

    // Image specific
    uint32_t width_px = 0;
    uint32_t height_px = 0;
    uint8_t num_channels = 0;

  public:
    ImageResourceBuilder() = default;
    ~ImageResourceBuilder() = default;
    ImageResourceBuilder(const ImageResourceBuilder &) = delete;
    ImageResourceBuilder &operator=(const ImageResourceBuilder &) = delete;

    ImageResourceBuilder &name(const std::string &&name) {
        name_ = std::move(name);
        return *this;
    }

    ImageResourceBuilder &length(const unsigned int length) {
        length_ = length;
        return *this;
    }

    ImageResourceBuilder &bytes(unsigned char *bytes) {
        bytes_ = bytes;
        return *this;
    }

    ImageResourceBuilder &width(const uint32_t width) {
        width_px = width;
        return *this;
    }

    ImageResourceBuilder &height(const uint32_t height) {
        height_px = height;
        return *this;
    }

    ImageResourceBuilder &channels(const uint8_t num_channels) {
        this->num_channels = num_channels;
        return *this;
    }

    std::unique_ptr<ImageResource> build() {
        // TODO: enforce required params
        return std::unique_ptr<ImageResource>(
            new ImageResource(std::move(name_), length_, static_cast<uint8_t *>(bytes_),
                              width_px, height_px, num_channels));
    }
};
