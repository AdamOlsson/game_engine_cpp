#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <cstddef>
#include <memory>

class Fence {
  private:
    std::shared_ptr<CoreGraphicsContext> ctx;
    size_t size;

    size_t next;
    std::vector<VkFence> fences;

  public:
    Fence(std::shared_ptr<CoreGraphicsContext> ctx, const size_t size);
    ~Fence();
    /*Fence(const Fence &);*/
    /*Fence &operator=(const Fence &);*/

    const VkFence get();
};
