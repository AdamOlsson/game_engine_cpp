#pragma once

#include "game_engine_sdk/render_engine/vulkan/GraphicsContext.h"
#include <memory>
namespace vulkan {
class ImageView {
  private:
    std::shared_ptr<vulkan::GraphicsContext> m_ctx;
    VkImageView m_image_view;

    VkImageView create_image_view(const VkImage &image, const VkFormat &format);

  public:
    ImageView();
    ImageView(std::shared_ptr<vulkan::GraphicsContext> ctx, const VkImage &image,
              const VkFormat format);
    ~ImageView();

    ImageView(ImageView &&other) noexcept;
    ImageView &operator=(ImageView &&other) noexcept;
    ImageView(const ImageView &other) = delete;
    ImageView &operator=(const ImageView &other) = delete;

    operator VkImageView() const { return m_image_view; }
};
} // namespace vulkan
