#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <memory>
class Texture {
  private:
    std::shared_ptr<CoreGraphicsContext> ctx;
    VkImage texture_image;
    VkDeviceMemory texture_image_memory;
    VkImageView texture_image_view;

  public:
    Texture();
    Texture(std::shared_ptr<CoreGraphicsContext> ctx, const VkCommandPool &command_pool,
            const VkQueue &graphics_queue, const char *filepath);

    ~Texture();

    Texture(Texture &&other) noexcept;
    Texture &operator=(Texture &&other) noexcept;

    Texture(const Texture &other) = delete;
    Texture &operator=(const Texture &other) = delete;

    VkImageView view();
};
