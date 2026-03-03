#pragma once
#include "vulkan/context/GraphicsContext.h"
#include <vulkan/vulkan.h>

namespace vulkan {

class RenderPassHandle {
  public:
    RenderPassHandle() = default;
    RenderPassHandle(std::shared_ptr<context::GraphicsContext> ctx, VkRenderPass pass)
        : m_ctx(ctx), m_pass(pass) {}

    ~RenderPassHandle() {
        if (m_pass != VK_NULL_HANDLE)
            vkDestroyRenderPass(m_ctx->logical_device, m_pass, nullptr);
    }

    RenderPassHandle(RenderPassHandle &&o) noexcept
        : m_ctx(std::move(o.m_ctx)), m_pass(o.m_pass) {
        o.m_pass = VK_NULL_HANDLE;
    }
    RenderPassHandle &operator=(RenderPassHandle &&o) noexcept {
        if (this != &o) {
            if (m_pass != VK_NULL_HANDLE) {
                vkDestroyRenderPass(m_ctx->logical_device, m_pass, nullptr);
            }
            m_ctx = std::move(o.m_ctx);
            m_pass = o.m_pass;
            o.m_pass = VK_NULL_HANDLE;
        }
        return *this;
    }
    RenderPassHandle(const RenderPassHandle &) = delete;
    RenderPassHandle &operator=(const RenderPassHandle &) = delete;

    VkRenderPass get() const { return m_pass; }
    operator VkRenderPass() const { return m_pass; }

  private:
    std::shared_ptr<context::GraphicsContext> m_ctx;
    VkRenderPass m_pass = VK_NULL_HANDLE;
};

} // namespace vulkan
