#pragma once

#include "game_engine_sdk/render_engine/CommandBufferManager.h"
#include "game_engine_sdk/render_engine/ImageData.h"
#include "game_engine_sdk/render_engine/graphics_context/GraphicsContext.h"
#include "game_engine_sdk/render_engine/vulkan/ImageView.h"
#include "vulkan/vulkan_core.h"

namespace vulkan {
struct TextureImageDimension {
    unsigned int width;
    unsigned int height;

    static TextureImageDimension from(const ImageDimension &image_dim) {
        return TextureImageDimension{image_dim.width, image_dim.height};
    }
};

class TextureImage {
  private:
    const VkFormat m_format = VK_FORMAT_R8G8B8A8_SRGB;
    std::shared_ptr<graphics_context::GraphicsContext> m_ctx;

    void destroy();

  public:
    VkImage m_image;
    VkDeviceMemory m_image_memory;
    TextureImageDimension m_dimension;

    TextureImage();
    TextureImage(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                 const TextureImageDimension &dim);
    TextureImage(TextureImage &&other) noexcept;
    TextureImage(const TextureImage &other) = delete;
    TextureImage &operator=(TextureImage &&other) noexcept;
    TextureImage &operator=(const TextureImage &other) = delete;

    ~TextureImage();

    void transition_image_layout(CommandBufferManager *command_buffer_manager,
                                 const VkImageLayout old_layout,
                                 const VkImageLayout new_layout);

    ImageView create_view();
};
} // namespace vulkan
