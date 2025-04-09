#include "FrameBuffer.h"
#include "vulkan/vulkan_core.h"

std::vector<VkFramebuffer> create_frame_buffers(std::shared_ptr<CoreGraphicsContext> &ctx,
                                                const SwapChain &swap_chain,
                                                const VkRenderPass &render_pass);

FrameBuffer::FrameBuffer(std::shared_ptr<CoreGraphicsContext> ctx,
                         const SwapChain &swap_chain, const VkRenderPass &render_pass)
    : ctx(ctx), next(0), buffers(create_frame_buffers(ctx, swap_chain, render_pass)) {}

FrameBuffer::~FrameBuffer() {
    for (size_t i = 0; i < buffers.size(); i++) {
        vkDestroyFramebuffer(ctx->device, buffers[i], nullptr);
    }
}

VkFramebuffer FrameBuffer::get() {
    auto &buf = buffers[next];
    next = ++next % buffers.size();
    return buf;
}

std::vector<VkFramebuffer> create_frame_buffers(std::shared_ptr<CoreGraphicsContext> &ctx,
                                                const SwapChain &swap_chain,
                                                const VkRenderPass &render_pass) {

    const size_t capacity = swap_chain.swapChainImageViews.size();
    std::vector<VkFramebuffer> swapChainFramebuffers;
    swapChainFramebuffers.resize(capacity);

    for (size_t i = 0; i < capacity; i++) {
        VkImageView attachments[] = {swap_chain.swapChainImageViews[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = render_pass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swap_chain.swapChainExtent.width;
        framebufferInfo.height = swap_chain.swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(ctx->device, &framebufferInfo, nullptr,
                                &swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
    return swapChainFramebuffers;
}
