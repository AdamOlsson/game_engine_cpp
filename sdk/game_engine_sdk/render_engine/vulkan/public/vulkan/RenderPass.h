#pragma once

#include "vulkan/Format.h"
#include "vulkan/RenderPassOpts.h"
#include "vulkan/context/GraphicsContext.h"
#include <memory>
namespace vulkan {

class RenderPass {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;
    VkRenderPass m_render_pass = VK_NULL_HANDLE;

  public:
    RenderPass() = default;
    RenderPass(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
               vulkan::Format format, const RenderPassOpts &opts = RenderPassOpts{});

    RenderPass(RenderPass &&other) noexcept;
    RenderPass &operator=(RenderPass &&other) noexcept;
    RenderPass(const RenderPass &) = delete;
    RenderPass &operator=(const RenderPass &) = delete;

    ~RenderPass();

    operator VkRenderPass() const { return m_render_pass; }
};

} // namespace vulkan
