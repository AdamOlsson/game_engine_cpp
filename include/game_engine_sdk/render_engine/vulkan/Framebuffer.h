#pragma once

#include "game_engine_sdk/render_engine/vulkan/GraphicsContext.h"
#include "game_engine_sdk/render_engine/vulkan/ImageView.h"
#include <memory>
namespace vulkan {
class Framebuffer {
  private:
    std::shared_ptr<vulkan::GraphicsContext> m_ctx;
    VkFramebuffer m_frame_buffer;

    VkFramebuffer create_framebuffers(ImageView &view, VkRenderPass &render_pass,
                                      VkExtent2D &extent);

  public:
    Framebuffer();
    Framebuffer(std::shared_ptr<vulkan::GraphicsContext> ctx, ImageView &view,
                VkRenderPass &render_pass, VkExtent2D &extent);

    ~Framebuffer();

    Framebuffer(Framebuffer &&other) noexcept;
    Framebuffer &operator=(Framebuffer &&other) noexcept;
    Framebuffer(const Framebuffer &other) = delete;
    Framebuffer &operator=(const Framebuffer &other) = delete;

    operator VkFramebuffer() const { return m_frame_buffer; }
};
} // namespace vulkan
