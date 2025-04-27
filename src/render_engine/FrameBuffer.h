#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/SwapChain.h"
#include <memory>

class FrameBuffer {
  private:
    std::shared_ptr<CoreGraphicsContext> m_ctx;

    size_t m_next;
    std::vector<VkFramebuffer> m_buffers;

  public:
    FrameBuffer(std::shared_ptr<CoreGraphicsContext> ctx, const SwapChain &swap_chain,
                const VkRenderPass &render_pass);

    FrameBuffer &operator=(const FrameBuffer &) = delete;
    FrameBuffer(const FrameBuffer &) = delete;

    FrameBuffer &operator=(FrameBuffer &&) noexcept;
    FrameBuffer(FrameBuffer &&) noexcept;
    ~FrameBuffer();

    VkFramebuffer get();
};
