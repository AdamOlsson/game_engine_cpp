#include "graphics_pipeline/ShaderResource.h"
/*#include
 * "game_engine_sdk/render_engine/resources/shaders/fragment/geometry/geometry.h"*/
/*#include "game_engine_sdk/render_engine/resources/shaders/fragment/text/text.h"*/
/*#include "game_engine_sdk/render_engine/resources/shaders/vertex/geometry/geometry.h"*/
/*#include "game_engine_sdk/render_engine/resources/shaders/vertex/text/text.h"*/
#include <memory>

graphics_pipeline::ShaderResource::ShaderResource(std::string &&name, unsigned int length,
                                                  uint8_t *bytes)
    : name_(std::move(name)), length_(length), bytes_(bytes) {}

const uint8_t *graphics_pipeline::ShaderResource::bytes() const { return bytes_; }

const std::string &graphics_pipeline::ShaderResource::name() const { return name_; }

const unsigned int graphics_pipeline::ShaderResource::length() const { return length_; }

std::vector<std::unique_ptr<graphics_pipeline::ShaderResource>>
graphics_pipeline::fetch_all_shaders() {
    std::vector<std ::unique_ptr<graphics_pipeline::ShaderResource>> shaders{};

    /*shaders.push_back(GeometryFragment::create_resource());*/
    /*shaders.push_back(GeometryVertex::create_resource());*/

    /*shaders.push_back(TextFragment::create_resource());*/
    /*shaders.push_back(TextVertex::create_resource());*/

    return shaders;
}
