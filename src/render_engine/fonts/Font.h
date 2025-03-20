#pragma once

#include "render_engine/Texture.h"
#include <cstddef>
#include <iostream>

enum class UseFont {
    None,
    Default,
};

struct FontDescription {
    std::string path;
    uint8_t char_width_px;
    uint8_t char_height_px;
    // These are technically already automatically read in texture loading, should we
    // do this differently?
    uint32_t atlas_width_px;
    uint32_t atlas_height_px;
};

const FontDescription default_font{
    .path = "assets/fonts/atlas.png",
    .char_width_px = 64,
    .char_height_px = 64,
    .atlas_width_px = 512,
    .atlas_height_px = 512,
};

class Font {
  private:
    float char_width_px;
    float char_height_px;
    float atlas_width_px;
    float atlas_height_px;
    uint32_t atlas_width;
    uint32_t atlas_height;

  public:
    std::unique_ptr<Texture> font_atlas;

    Font()
        : char_width_px(0), char_height_px(0), font_atlas(nullptr), atlas_width_px(0),
          atlas_height_px(0), atlas_width(0), atlas_height(0) {}

    Font(std::shared_ptr<CoreGraphicsContext> &g_ctx, const VkCommandPool &command_pool,
         const VkQueue &graphics_queue, const FontDescription &description)
        : char_width_px(description.char_width_px),
          char_height_px(description.char_height_px),
          atlas_width_px(description.atlas_width_px),
          atlas_height_px(description.atlas_height_px),
          atlas_width(atlas_width_px / char_width_px),
          atlas_height(atlas_height_px / char_height_px),
          font_atlas(std::make_unique<Texture>(g_ctx, command_pool, graphics_queue,
                                               description.path.c_str())) {}

    ~Font() = default;

    Font(Font &&other) noexcept
        : char_width_px(other.char_width_px), char_height_px(other.char_height_px),
          font_atlas(std::move(other.font_atlas)), atlas_width_px(other.atlas_width_px),
          atlas_height_px(other.atlas_height_px), atlas_width(other.atlas_width),
          atlas_height(other.atlas_height) {
        other.char_width_px = 0;
        other.char_height_px = 0;
        other.atlas_width_px = 0;
        other.atlas_height_px = 0;
        other.atlas_width = 0;
        other.atlas_height = 0;
        other.font_atlas = nullptr;
    };

    Font &operator=(Font &&other) {
        if (this != &other) {
            char_width_px = other.char_width_px;
            char_height_px = other.char_height_px;
            atlas_width_px = other.atlas_width_px;
            atlas_height_px = other.atlas_height_px;
            atlas_width = other.atlas_width;
            atlas_height = other.atlas_height;
            font_atlas = std::move(other.font_atlas);

            other.char_width_px = 0;
            other.char_height_px = 0;
            other.atlas_width_px = 0;
            other.atlas_height_px = 0;
            other.atlas_width = 0;
            other.atlas_height = 0;
            other.font_atlas = nullptr;
        }
        return *this;
    };

    Font &operator=(const Font &other) = delete;
    Font(const Font &other) = delete;

    /* Encode ascii characters to normalized uv coordinates.
     *
     * Calculates the normalized uvwt coordinates used to sample the correct character
     * in the fragment shader. Assumes that each token between uint8 value 32 to 95.
     */
    std::vector<glm::vec4> encode_ascii(const std::string &text) {
        const uint32_t atlas_width =
            atlas_width_px / char_width_px; // num characters in width
        const uint32_t atlas_height =
            atlas_height_px / char_height_px; // num characters in height

        std::vector<glm::vec4> encoded{};
        encoded.reserve(text.size());
        for (const char &c : text) {
            uint32_t index = c - 32;
            uint32_t col = index % atlas_width;
            uint32_t row = index / atlas_height;
            encoded.push_back(encode_ascii_char(c));
        }
        return encoded;
    }

    glm::vec4 encode_ascii_char(const char &c) {
        uint32_t index = c - 32;
        uint32_t col = index % atlas_width;
        uint32_t row = index / atlas_height;
        return glm::vec4(
            // top left
            col * char_width_px / atlas_width_px, row * char_height_px / atlas_height_px,
            // bottom right
            (col + 1) * char_width_px / atlas_width_px,
            (row + 1) * char_height_px / atlas_height_px);
    }
};
