#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/SwapChain.h"
#include <memory>

class FrameBuffer {
  private:
    std::shared_ptr<CoreGraphicsContext> ctx;

    size_t next;
    std::vector<VkFramebuffer> buffers;

    /*FrameBuffer &operator=(const FrameBuffer &); // Copy assignment*/
    /*FrameBuffer &operator=(FrameBuffer &&) noexcept; // Move assignment*/
    /*FrameBuffer(const FrameBuffer &);     // Copy*/
    /*FrameBuffer(FrameBuffer &&) noexcept; // Move*/

  public:
    FrameBuffer(std::shared_ptr<CoreGraphicsContext> ctx, const SwapChain &swap_chain,
                const VkRenderPass &render_pass);
    ~FrameBuffer();

    VkFramebuffer get();
};
