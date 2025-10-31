#include "vulkan/Framebuffer.h"

vulkan::Framebuffer::Framebuffer() : m_frame_buffer(VK_NULL_HANDLE) {}

vulkan::Framebuffer::Framebuffer(std::shared_ptr<vulkan::context::GraphicsContext> ctx,
                                 ImageView &view, VkRenderPass &render_pass,
                                 VkExtent2D &extent)
    : m_ctx(ctx), m_frame_buffer(create_framebuffers(view, render_pass, extent)) {}

vulkan::Framebuffer::~Framebuffer() {
    if (m_frame_buffer == VK_NULL_HANDLE) {
        return;
    }
    vkDestroyFramebuffer(m_ctx->logical_device, m_frame_buffer, nullptr);
    m_frame_buffer = VK_NULL_HANDLE;
}

vulkan::Framebuffer::Framebuffer(Framebuffer &&other) noexcept
    : m_ctx(std::move(other.m_ctx)), m_frame_buffer(other.m_frame_buffer) {
    other.m_frame_buffer = VK_NULL_HANDLE;
}

vulkan::Framebuffer &vulkan::Framebuffer::operator=(Framebuffer &&other) noexcept {
    if (this != &other) {
        if (m_frame_buffer != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(m_ctx->logical_device, m_frame_buffer, nullptr);
        }

        m_ctx = std::move(other.m_ctx);
        m_frame_buffer = other.m_frame_buffer;

        other.m_frame_buffer = VK_NULL_HANDLE;
    }
    return *this;
}

VkFramebuffer vulkan::Framebuffer::create_framebuffers(ImageView &view,
                                                       VkRenderPass &render_pass,
                                                       VkExtent2D &extent) {
    VkImageView attachments[] = {view};

    VkFramebufferCreateInfo framebuffer_info{};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = render_pass;
    framebuffer_info.attachmentCount = 1;
    framebuffer_info.pAttachments = attachments;
    framebuffer_info.width = extent.width;
    framebuffer_info.height = extent.height;
    framebuffer_info.layers = 1;

    VkFramebuffer swap_chain_frame_buffer;
    if (vkCreateFramebuffer(m_ctx->logical_device, &framebuffer_info, nullptr,
                            &swap_chain_frame_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create framebuffer!");
    }
    return swap_chain_frame_buffer;
}
