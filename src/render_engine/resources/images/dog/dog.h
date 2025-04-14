#pragma once

#include "render_engine/resources/images/ImageResource.h"
#include <memory>

class DogImage {
  public:
    static std::unique_ptr<ImageResource> create_resource();
};
