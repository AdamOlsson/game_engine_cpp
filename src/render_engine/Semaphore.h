#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <cstddef>
#include <memory>

class Semaphore {
  private:
    std::shared_ptr<CoreGraphicsContext> ctx;
    size_t size;

    size_t next;
    std::vector<VkSemaphore> semaphores;

    /*Semaphore(const Semaphore &);*/
    /*Semaphore &operator=(const Semaphore &);*/

  public:
    Semaphore(std::shared_ptr<CoreGraphicsContext> ctx, const size_t size);
    ~Semaphore();

    const VkSemaphore get();
};
