#pragma once
#include "game_engine_sdk/render_engine/resources/fonts/FontResource.h"
#include <memory>

class DefaultFont {
  public:
    static std::unique_ptr<FontResource> create_resource();
};
