#include "DescriptorSetLayout.h"
#include "vulkan/vulkan_core.h"

DescriptorSetLayout::DescriptorSetLayout(
    std::shared_ptr<graphics_context::GraphicsContext> ctx, VkDescriptorSetLayout &layout)
    : m_ctx(ctx), m_layout(layout) {}

DescriptorSetLayout::~DescriptorSetLayout() {
    vkDestroyDescriptorSetLayout(m_ctx->logical_device, m_layout, nullptr);
}
