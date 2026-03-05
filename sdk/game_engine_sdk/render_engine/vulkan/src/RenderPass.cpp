#include "vulkan/RenderPass.h"

namespace vulkan {

RenderPass::RenderPass(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                       vulkan::Format format, const RenderPassOpts &opts)
    : m_ctx(ctx) {

    VkAttachmentDescription color_attachment{};
    color_attachment.format = format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = opts.color.load_op;
    color_attachment.storeOp = opts.color.store_op;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = opts.color.initial_layout;
    color_attachment.finalLayout = opts.color.final_layout;

    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    if (vkCreateRenderPass(m_ctx->logical_device, &render_pass_info, nullptr,
                           &m_render_pass) != VK_SUCCESS) {
        throw std::runtime_error("Error: Failed to create render pass.");
    }
}

RenderPass::~RenderPass() {
    if (m_render_pass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(m_ctx->logical_device, m_render_pass, nullptr);
    }
    m_render_pass = VK_NULL_HANDLE;
}

RenderPass::RenderPass(RenderPass &&other) noexcept
    : m_ctx(std::move(other.m_ctx)), m_render_pass(std::move(other.m_render_pass)) {
    other.m_render_pass = VK_NULL_HANDLE;
    other.m_ctx = nullptr;
}

RenderPass &RenderPass::operator=(RenderPass &&other) noexcept {
    if (this != &other) {
        if (m_render_pass != VK_NULL_HANDLE) {
            vkDestroyRenderPass(m_ctx->logical_device, m_render_pass, nullptr);
        }
        m_ctx = std::move(other.m_ctx);
        m_render_pass = std::move(other.m_render_pass);

        other.m_render_pass = VK_NULL_HANDLE;
        other.m_ctx = nullptr;
    }
    return *this;
}

} // namespace vulkan
