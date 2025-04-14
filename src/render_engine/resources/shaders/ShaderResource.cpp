#include "ShaderResource.h"
#include "render_engine/resources/shaders/geometry_fragment/geometry_fragment.h"
#include "render_engine/resources/shaders/text_fragment/text_fragment.h"
#include "render_engine/resources/shaders/vert/vert.h"
#include <memory>

ShaderResource::ShaderResource(const std::string &&name, const unsigned int length,
                               const uint8_t *bytes)
    : name_(std::move(name)), length_(length), bytes_(bytes) {}

const uint8_t *ShaderResource::bytes() const { return bytes_; }

const std::string &ShaderResource::name() const { return name_; }

const unsigned int ShaderResource::length() const { return length_; }

std::vector<std::unique_ptr<ShaderResource>> fetch_all_shaders() {
    std::vector<std ::unique_ptr<ShaderResource>> fonts{};
    fonts.push_back(GeometryFragment::create_resource());
    fonts.push_back(TextFragment::create_resource());
    fonts.push_back(Vert::create_resource());
    return fonts;
}
