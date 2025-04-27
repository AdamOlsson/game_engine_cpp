#include "FrameBuffer.h"
#include "vulkan/vulkan_core.h"

std::vector<VkFramebuffer> create_frame_buffers(std::shared_ptr<CoreGraphicsContext> &ctx,
                                                const SwapChain &swap_chain,
                                                const VkRenderPass &render_pass);

FrameBuffer::FrameBuffer(std::shared_ptr<CoreGraphicsContext> ctx,
                         const SwapChain &swap_chain, const VkRenderPass &render_pass)
    : m_ctx(ctx), m_next(0),
      m_buffers(create_frame_buffers(ctx, swap_chain, render_pass)) {}

FrameBuffer::~FrameBuffer() {
    for (size_t i = 0; i < m_buffers.size(); i++) {
        vkDestroyFramebuffer(m_ctx->device, m_buffers[i], nullptr);
    }
}

FrameBuffer::FrameBuffer(FrameBuffer &&other) noexcept
    : m_ctx(std::move(other.m_ctx)), m_next(other.m_next),
      m_buffers(std::move(other.m_buffers)) {}

FrameBuffer &FrameBuffer::operator=(FrameBuffer &&other) noexcept {
    if (this != &other) {
        m_ctx = std::move(other.m_ctx);
        m_next = other.m_next;
        m_buffers = std::move(other.m_buffers);
    }
    return *this;
}

VkFramebuffer FrameBuffer::get() {
    auto &buf = m_buffers[m_next];
    m_next = ++m_next % m_buffers.size();
    return buf;
}

std::vector<VkFramebuffer> create_frame_buffers(std::shared_ptr<CoreGraphicsContext> &ctx,
                                                const SwapChain &swap_chain,
                                                const VkRenderPass &render_pass) {

    const size_t capacity = swap_chain.image_views.size();
    std::vector<VkFramebuffer> swapChainFramebuffers;
    swapChainFramebuffers.resize(capacity);

    for (size_t i = 0; i < capacity; i++) {
        VkImageView attachments[] = {swap_chain.image_views[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = render_pass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swap_chain.extent.width;
        framebufferInfo.height = swap_chain.extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(ctx->device, &framebufferInfo, nullptr,
                                &swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
    return swapChainFramebuffers;
}
