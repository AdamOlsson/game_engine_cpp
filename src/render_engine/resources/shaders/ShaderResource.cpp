#include "ShaderResource.h"
#include "render_engine/resources/shaders/fragment/geometry/geometry.h"
#include "render_engine/resources/shaders/fragment/text/text.h"
#include "render_engine/resources/shaders/fragment/ui/ui.h"
#include "render_engine/resources/shaders/vertex/geometry/geometry.h"
#include "render_engine/resources/shaders/vertex/ui/ui.h"
#include <memory>

ShaderResource::ShaderResource(const std::string &&name, const unsigned int length,
                               const uint8_t *bytes)
    : name_(std::move(name)), length_(length), bytes_(bytes) {}

const uint8_t *ShaderResource::bytes() const { return bytes_; }

const std::string &ShaderResource::name() const { return name_; }

const unsigned int ShaderResource::length() const { return length_; }

std::vector<std::unique_ptr<ShaderResource>> fetch_all_shaders() {
    std::vector<std ::unique_ptr<ShaderResource>> shaders{};
    shaders.push_back(GeometryFragment::create_resource());
    shaders.push_back(GeometryVertex::create_resource());
    shaders.push_back(TextFragment::create_resource());
    shaders.push_back(UiFragment::create_resource());
    shaders.push_back(UiVertex::create_resource());
    return shaders;
}
