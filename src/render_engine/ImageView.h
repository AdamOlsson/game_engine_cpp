#pragma once

#include "render_engine/graphics_context/GraphicsContext.h"
#include <memory>
class ImageView {
  private:
    std::shared_ptr<graphics_context::GraphicsContext> m_ctx;
    VkImageView m_image_view;

    VkImageView create_image_view(const VkImage &image, const VkFormat &format);

  public:
    ImageView();
    ImageView(std::shared_ptr<graphics_context::GraphicsContext> ctx,
              const VkImage &image, const VkFormat format);
    ~ImageView();

    ImageView(ImageView &&other) noexcept;
    ImageView &operator=(ImageView &&other) noexcept;
    ImageView(const ImageView &other) = delete;
    ImageView &operator=(const ImageView &other) = delete;

    operator VkImageView() const { return m_image_view; }
};
