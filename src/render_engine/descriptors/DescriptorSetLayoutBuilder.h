#pragma once

#include "render_engine/descriptors/DescriptorSetLayout.h"
#include "render_engine/graphics_context/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <vector>
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

    DescriptorSetLayoutBuilder &add_combined_image_sampler_binding(uint32_t binding);
    DescriptorSetLayoutBuilder &add_storage_buffer_binding(uint32_t binding);
    DescriptorSetLayoutBuilder &add_uniform_buffer_binding(uint32_t binding);
    DescriptorSetLayout build(std::shared_ptr<graphics_context::GraphicsContext> &ctx);
};
