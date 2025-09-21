#pragma once

#include "game_engine_sdk/render_engine/vulkan/ImageView.h"
#include "game_engine_sdk/render_engine/vulkan/Sampler.h"
#include "vulkan/vulkan_core.h"

namespace vulkan {
class DescriptorImageInfo {
  private:
    VkDescriptorImageInfo m_descriptor_image_info;

  public:
    DescriptorImageInfo() = default;
    DescriptorImageInfo(ImageView *image_view, Sampler *image_sampler);
    ~DescriptorImageInfo() = default;
    DescriptorImageInfo(DescriptorImageInfo &&other) noexcept = default;
    DescriptorImageInfo &operator=(DescriptorImageInfo &&other) noexcept = default;
    DescriptorImageInfo(const DescriptorImageInfo &other) = default;
    DescriptorImageInfo &operator=(const DescriptorImageInfo &other) = default;
    operator VkDescriptorImageInfo() const { return m_descriptor_image_info; }

    const VkDescriptorImageInfo *get() const { return &m_descriptor_image_info; }
};

} // namespace vulkan
