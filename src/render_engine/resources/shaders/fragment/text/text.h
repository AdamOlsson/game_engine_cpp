#pragma once
#include "render_engine/resources/shaders/ShaderResource.h"
#include <memory>

class TextFragment {
  public:
    static std::unique_ptr<ShaderResource> create_resource();
};
