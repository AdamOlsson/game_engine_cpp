#pragma once

#include "game_engine_sdk/render_engine/Texture.h"
#include "game_engine_sdk/render_engine/fonts/KerningMap.h"
#include "game_engine_sdk/render_engine/resources/ResourceManager.h"
#include "game_engine_sdk/render_engine/resources/fonts/FontResource.h"
#include "vulkan/Sampler.h"
#include <cstddef>

class Font {
  private:
    float char_width_px;
    float char_height_px;
    float atlas_width_px;
    float atlas_height_px;
    uint32_t atlas_width;
    uint32_t atlas_height;

  public:
    Texture font_atlas;
    vulkan::Sampler *sampler;
    font::KerningMap kerning_map = font::get_default_kerning_map();

    Font() = default;

    Font(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
         vulkan::CommandBufferManager *command_buffer_manager,
         const std::string &font_name, vulkan::Sampler *sampler)
        : sampler(sampler) {
        auto resource =
            ResourceManager::get_instance().get_resource<FontResource>("DefaultFont");
        char_width_px = resource->char_width_px;
        char_height_px = resource->char_height_px;
        atlas_width_px = resource->atlas_width_px;
        atlas_height_px = resource->atlas_height_px;
        atlas_width = atlas_width_px / char_width_px;
        atlas_height = atlas_height_px / char_height_px;
        font_atlas = Texture::from_bytes(ctx, command_buffer_manager, resource->bytes(),
                                         resource->length());
    }

    ~Font() = default;

    Font(Font &&other) noexcept = default;
    Font &operator=(Font &&other) noexcept = default;

    Font(const Font &other) = delete;
    Font &operator=(const Font &other) = delete;

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
