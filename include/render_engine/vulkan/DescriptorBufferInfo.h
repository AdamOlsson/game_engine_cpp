
#pragma once

#include "render_engine/buffers/GpuBuffer.h"
#include "vulkan/vulkan_core.h"

namespace vulkan {
class DescriptorBufferInfo {
  private:
    VkDescriptorBufferInfo m_descriptor_buffer_info;

  public:
    DescriptorBufferInfo() = default;
    DescriptorBufferInfo(GpuBufferRef &buffer_ref);
    ~DescriptorBufferInfo() = default;
    DescriptorBufferInfo(DescriptorBufferInfo &&other) noexcept = default;
    DescriptorBufferInfo &operator=(DescriptorBufferInfo &&other) noexcept = default;
    DescriptorBufferInfo(const DescriptorBufferInfo &other) = default;
    DescriptorBufferInfo &operator=(const DescriptorBufferInfo &other) = default;
    operator VkDescriptorBufferInfo() const { return m_descriptor_buffer_info; }

    const VkDescriptorBufferInfo *get() const { return &m_descriptor_buffer_info; }

    static std::vector<DescriptorBufferInfo>
    from_vector(std::vector<GpuBufferRef> &&buffer_refs);
};

} // namespace vulkan
