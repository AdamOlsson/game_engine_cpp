#include "game_engine_sdk/render_engine/descriptors/DescriptorSetLayoutBuilder.h"
#include "vulkan/vulkan_core.h"

DescriptorSetLayoutBuilder &
DescriptorSetLayoutBuilder::add(const VkDescriptorSetLayoutBinding &&binding) {
    m_bindings.push_back(std::move(binding));
    return *this;
}

vulkan::DescriptorSetLayout DescriptorSetLayoutBuilder::build(
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
    return vulkan::DescriptorSetLayout(ctx, layout);
}

DescriptorSetLayoutBuilder &
DescriptorSetLayoutBuilder::add_combined_image_sampler_binding(uint32_t binding,
                                                               uint32_t count) {
    VkDescriptorSetLayoutBinding sampler_layout_binding{};
    sampler_layout_binding.binding = binding;
    sampler_layout_binding.descriptorCount = count;
    sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    sampler_layout_binding.pImmutableSamplers = nullptr;
    sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    m_bindings.push_back(sampler_layout_binding);
    return *this;
}

DescriptorSetLayoutBuilder &
DescriptorSetLayoutBuilder::add_storage_buffer_binding(uint32_t binding,
                                                       VkShaderStageFlags stage_flags) {
    VkDescriptorSetLayoutBinding layout_binding{};
    layout_binding.binding = binding;
    layout_binding.descriptorCount = 1;
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    layout_binding.pImmutableSamplers = nullptr;
    layout_binding.stageFlags = stage_flags;
    m_bindings.push_back(layout_binding);
    return *this;
}

DescriptorSetLayoutBuilder &
DescriptorSetLayoutBuilder::add_uniform_buffer_binding(uint32_t binding,
                                                       VkShaderStageFlags stage_flags) {
    VkDescriptorSetLayoutBinding layout_binding{};
    layout_binding.binding = binding;
    layout_binding.descriptorCount = 1;
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layout_binding.pImmutableSamplers = nullptr;
    layout_binding.stageFlags = stage_flags;

    m_bindings.push_back(layout_binding);
    return *this;
}
