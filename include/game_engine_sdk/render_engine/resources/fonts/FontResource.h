#pragma once

#include "game_engine_sdk/render_engine/resources/Resource.h"
#include <cstdint>
#include <memory>
#include <vector>

class FontResource : public Resource {

  private:
    // All resources
    const std::string name_;
    const unsigned int length_;
    const uint8_t *bytes_;

    FontResource(const std::string &&name, const unsigned int length,
                 const uint8_t *bytes, const uint8_t char_width_px,
                 const uint8_t char_height_px, const uint32_t atlas_width_px,
                 const uint32_t atlas_height_px);

    friend class FontResourceBuilder;

  public:
    // Font specific
    const uint8_t char_width_px;
    const uint8_t char_height_px;
    const uint32_t atlas_width_px;
    const uint32_t atlas_height_px;

    const uint8_t *bytes() const override;
    const std::string &name() const override;
    const unsigned int length() const override;
};

std::vector<std::unique_ptr<FontResource>> fetch_all_fonts();

class FontResourceBuilder {
  private:
    // All resources
    std::string name_;
    unsigned int length_ = 0;
    unsigned char *bytes_ = nullptr;

    // Font specific
    uint8_t char_width_px = 0;
    uint8_t char_height_px = 0;
    uint32_t atlas_width_px = 0;
    uint32_t atlas_height_px = 0;

  public:
    FontResourceBuilder() = default;
    ~FontResourceBuilder() = default;
    FontResourceBuilder(const FontResourceBuilder &) = delete;
    FontResourceBuilder &operator=(const FontResourceBuilder &) = delete;

    FontResourceBuilder &name(const std::string &&name) {
        name_ = std::move(name);
        return *this;
    }

    FontResourceBuilder &length(const unsigned int length) {
        length_ = length;
        return *this;
    }

    FontResourceBuilder &bytes(unsigned char *bytes) {
        bytes_ = bytes;
        return *this;
    }

    FontResourceBuilder &char_width(const uint8_t width) {
        char_width_px = width;
        return *this;
    }

    FontResourceBuilder &char_height(const uint8_t height) {
        char_height_px = height;
        return *this;
    }

    FontResourceBuilder &atlas_width(const uint32_t width) {
        atlas_width_px = width;
        return *this;
    }

    FontResourceBuilder &atlas_height(const uint32_t height) {
        atlas_height_px = height;
        return *this;
    }

    std::unique_ptr<FontResource> build() {
        return std::unique_ptr<FontResource>(new FontResource(
            std::move(name_), length_, static_cast<uint8_t *>(bytes_), char_width_px,
            char_height_px, atlas_width_px, atlas_height_px));
    }
};
