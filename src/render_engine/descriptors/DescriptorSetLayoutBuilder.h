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

    DescriptorSetLayoutBuilder &add(const VkDescriptorSetLayoutBinding &&binding);

    DescriptorSetLayout build(std::shared_ptr<graphics_context::GraphicsContext> &ctx);
};
