#pragma once

#include "render_engine/graphics_context/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <vector>
class DescriptorSetLayoutBuilder {
  private:
    std::vector<VkDescriptorSetLayoutBinding> m_bindings;

  public:
    DescriptorSetLayoutBuilder() = default;
    ~DescriptorSetLayoutBuilder() = default;

    DescriptorSetLayoutBuilder &add(const VkDescriptorSetLayoutBinding &&binding);

    VkDescriptorSetLayout build(const graphics_context::GraphicsContext *ctx);
};
