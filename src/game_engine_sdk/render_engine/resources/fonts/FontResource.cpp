#include "game_engine_sdk/render_engine/resources/fonts/FontResource.h"
#include "default/default.h"
#include <memory>

FontResource::FontResource(const std::string &&name, const unsigned int length,
                           const uint8_t *bytes, const uint8_t char_width_px,
                           const uint8_t char_height_px, const uint32_t atlas_width_px,
                           const uint32_t atlas_height_px)
    : name_(std::move(name)), length_(length), bytes_(bytes),
      char_width_px(char_width_px), char_height_px(char_height_px),
      atlas_width_px(atlas_width_px), atlas_height_px(atlas_height_px) {}

const uint8_t *FontResource::bytes() const { return bytes_; }

const std::string &FontResource::name() const { return name_; }

const unsigned int FontResource::length() const { return length_; }

std::vector<std::unique_ptr<FontResource>> fetch_all_fonts() {
    std::vector<std ::unique_ptr<FontResource>> fonts{};
    fonts.push_back(DefaultFont::create_resource());

    return fonts;
}
