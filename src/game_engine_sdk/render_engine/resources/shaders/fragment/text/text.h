#pragma once
#include "graphics_pipeline/ShaderResource.h"
#include <memory>

class TextFragment {
  public:
  static std::unique_ptr<graphics_pipeline::ShaderResource> create_resource();
};
