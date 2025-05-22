#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <vector>
class DescriptorSetLayoutBuilder {
  private:
    std::vector<VkDescriptorSetLayoutBinding> m_bindings;

  public:
    DescriptorSetLayoutBuilder() = default;
    ~DescriptorSetLayoutBuilder() = default;

    DescriptorSetLayoutBuilder &add(const VkDescriptorSetLayoutBinding &&binding);

    VkDescriptorSetLayout build(const CoreGraphicsContext *ctx);
};
