#pragma once
#include "game_engine_sdk/render_engine/resources/images/ImageResource.h"
#include <memory>

class DogImage {
  public:
    static std::unique_ptr<ImageResource> create_resource();
};
