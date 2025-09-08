#include "ShaderResource.h"
#include "render_engine/resources/shaders/fragment/geometry/geometry.h"
#include "render_engine/resources/shaders/fragment/text/text.h"
#include "render_engine/resources/shaders/vertex/geometry/geometry.h"
#include "render_engine/resources/shaders/vertex/text/text.h"
#include <memory>

ShaderResource::ShaderResource(std::string &&name, unsigned int length, uint8_t *bytes)
    : name_(std::move(name)), length_(length), bytes_(bytes) {}

const uint8_t *ShaderResource::bytes() const { return bytes_; }

const std::string &ShaderResource::name() const { return name_; }

const unsigned int ShaderResource::length() const { return length_; }

std::vector<std::unique_ptr<ShaderResource>> fetch_all_shaders() {
    std::vector<std ::unique_ptr<ShaderResource>> shaders{};

    shaders.push_back(GeometryFragment::create_resource());
    shaders.push_back(GeometryVertex::create_resource());

    shaders.push_back(TextFragment::create_resource());
    shaders.push_back(TextVertex::create_resource());

    return shaders;
}
