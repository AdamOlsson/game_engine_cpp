#pragma once

#include "vulkan/DescriptorSetLayout.h"
#include "vulkan/context/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <vector>
namespace graphics_pipeline {
class DescriptorSetLayoutBuilder {
  private:
    std::vector<VkDescriptorSetLayoutBinding> m_bindings;

  public:
    DescriptorSetLayoutBuilder() = default;
    ~DescriptorSetLayoutBuilder() = default;

    DescriptorSetLayoutBuilder(DescriptorSetLayoutBuilder &&) noexcept = delete;
    DescriptorSetLayoutBuilder &
    operator=(DescriptorSetLayoutBuilder &&) noexcept = delete;
    DescriptorSetLayoutBuilder(const DescriptorSetLayoutBuilder &) = delete;
    DescriptorSetLayoutBuilder &operator=(const DescriptorSetLayoutBuilder &) = delete;

    DescriptorSetLayoutBuilder &add(const VkDescriptorSetLayoutBinding &&binding);

    DescriptorSetLayoutBuilder &add_combined_image_sampler_binding(uint32_t binding,
                                                                   uint32_t count);
    DescriptorSetLayoutBuilder &
    add_storage_buffer_binding(uint32_t binding, VkShaderStageFlags stage_flags);
    DescriptorSetLayoutBuilder &
    add_uniform_buffer_binding(uint32_t binding, VkShaderStageFlags stage_flags);
    vulkan::DescriptorSetLayout
    build(std::shared_ptr<vulkan::context::GraphicsContext> &ctx);
};
} // namespace graphics_pipeline
