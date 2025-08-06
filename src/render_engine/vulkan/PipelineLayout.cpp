#include "PipelineLayout.h"

vulkan::PipelineLayout::PipelineLayout(
    std::shared_ptr<graphics_context::GraphicsContext> ctx,
    const VkDescriptorSetLayout &descriptor_set_layout,
    const std::vector<VkPushConstantRange> &push_constant_range)
    : m_ctx(ctx), m_pipeline_layout(create_graphics_pipeline_layout(
                      descriptor_set_layout, push_constant_range)) {}

vulkan::PipelineLayout::~PipelineLayout() {
    if (m_pipeline_layout == VK_NULL_HANDLE) {
        return;
    }

    vkDestroyPipelineLayout(m_ctx->logical_device, m_pipeline_layout, nullptr);
    m_pipeline_layout = VK_NULL_HANDLE;
}

vulkan::PipelineLayout::PipelineLayout(PipelineLayout &&other) noexcept
    : m_ctx(std::move(other.m_ctx)), m_pipeline_layout(other.m_pipeline_layout) {
    other.m_pipeline_layout = VK_NULL_HANDLE;
}

vulkan::PipelineLayout &
vulkan::PipelineLayout::operator=(PipelineLayout &&other) noexcept {
    if (this != &other) {
        if (m_pipeline_layout != VK_NULL_HANDLE) {
            vkDestroyPipelineLayout(m_ctx->logical_device, m_pipeline_layout, nullptr);
        }
        m_ctx = std::move(other.m_ctx);
        m_pipeline_layout = other.m_pipeline_layout;

        other.m_pipeline_layout = VK_NULL_HANDLE;
    }
    return *this;
}

VkPipelineLayout vulkan::PipelineLayout::create_graphics_pipeline_layout(
    const VkDescriptorSetLayout &descriptor_set_layout,
    const std::vector<VkPushConstantRange> &push_constant_range) {

    VkPipelineLayoutCreateInfo pipeline_layout_into{};
    pipeline_layout_into.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_into.setLayoutCount = 1;
    pipeline_layout_into.pSetLayouts = &descriptor_set_layout;
    pipeline_layout_into.pushConstantRangeCount = push_constant_range.size();
    pipeline_layout_into.pPushConstantRanges = push_constant_range.data();

    VkPipelineLayout pipeline_layout;
    if (vkCreatePipelineLayout(m_ctx->logical_device, &pipeline_layout_into, nullptr,
                               &pipeline_layout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
    return pipeline_layout;
}
