#include "vulkan/DescriptorSetLayout.h"
#include "vulkan/vulkan_core.h"

vulkan::DescriptorSetLayout::DescriptorSetLayout(
    std::shared_ptr<vulkan::context::GraphicsContext> ctx, VkDescriptorSetLayout &layout)
    : m_ctx(ctx), m_layout(layout) {}

vulkan::DescriptorSetLayout::~DescriptorSetLayout() {
    if (m_layout == VK_NULL_HANDLE) {
        return;
    }
    vkDestroyDescriptorSetLayout(m_ctx->logical_device, m_layout, nullptr);
}

vulkan::DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout &&other) noexcept
    : m_ctx(std::move(other.m_ctx)), m_layout(other.m_layout) {
    other.m_layout = VK_NULL_HANDLE;
}

vulkan::DescriptorSetLayout &
vulkan::DescriptorSetLayout::operator=(DescriptorSetLayout &&other) noexcept {
    if (this != &other) {
        if (m_layout != VK_NULL_HANDLE && m_ctx) {
            vkDestroyDescriptorSetLayout(m_ctx->logical_device, m_layout, nullptr);
        }

        m_ctx = std::move(other.m_ctx);
        m_layout = other.m_layout;

        other.m_layout = VK_NULL_HANDLE;
    }
    return *this;
}
