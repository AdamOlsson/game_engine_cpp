#include "DescriptorSetLayoutBuilder.h"
#include "vulkan/vulkan_core.h"

DescriptorSetLayoutBuilder &
DescriptorSetLayoutBuilder::add(const VkDescriptorSetLayoutBinding &&binding) {
    m_bindings.push_back(std::move(binding));
    return *this;
}

DescriptorSetLayout DescriptorSetLayoutBuilder::build(
    std::shared_ptr<graphics_context::GraphicsContext> &ctx) {
    VkDescriptorSetLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = m_bindings.size();
    layout_info.pBindings = m_bindings.data();

    VkDescriptorSetLayout layout;
    if (vkCreateDescriptorSetLayout(ctx->logical_device, &layout_info, nullptr,
                                    &layout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
    return DescriptorSetLayout(ctx, layout);
}
