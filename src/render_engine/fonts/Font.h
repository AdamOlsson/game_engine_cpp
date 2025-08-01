#pragma once

#include "render_engine/SwapChainManager.h"
#include "render_engine/Texture.h"
#include "render_engine/fonts/KerningMap.h"
#include "render_engine/resources/fonts/FontResource.h"
#include <cstddef>

enum class UseFont {
    None,
    Default,
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
    const font::KerningMap kerning_map = font::get_default_kerning_map();

    Font()
        : char_width_px(0), char_height_px(0), font_atlas(nullptr), atlas_width_px(0),
          atlas_height_px(0), atlas_width(0), atlas_height(0) {}

    Font(std::shared_ptr<graphics_context::GraphicsContext> &g_ctx,
         SwapChainManager &swap_chain_manager, const FontResource *resource)

        : char_width_px(resource->char_width_px),
          char_height_px(resource->char_height_px),
          atlas_width_px(resource->atlas_width_px),
          atlas_height_px(resource->atlas_height_px),
          atlas_width(atlas_width_px / char_width_px),
          atlas_height(atlas_height_px / char_height_px),
          font_atlas(Texture::unique_from_bytes(g_ctx, swap_chain_manager,
                                                resource->bytes(), resource->length())) {}

    ~Font() = default;

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
