#pragma once
#include "game_engine_sdk/render_engine/resources/shaders/ShaderResource.h"
#include <memory>

class TextFragment {
  public:
    static std::unique_ptr<ShaderResource> create_resource();
};
