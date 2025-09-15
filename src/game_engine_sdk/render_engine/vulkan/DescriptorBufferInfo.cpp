#include "game_engine_sdk/render_engine/vulkan/DescriptorBufferInfo.h"

vulkan::DescriptorBufferInfo::DescriptorBufferInfo(GpuBufferRef &buffer_ref)
    : m_descriptor_buffer_info(VkDescriptorBufferInfo{
          .buffer = buffer_ref.buffer,
          .offset = 0,
          .range = buffer_ref.size,
      }) {}

std::vector<vulkan::DescriptorBufferInfo>
vulkan::DescriptorBufferInfo::from_vector(std::vector<GpuBufferRef> &&buffer_refs) {
    std::vector<vulkan::DescriptorBufferInfo> buffer_infos;
    buffer_infos.reserve(buffer_refs.size());
    for (auto &ref : buffer_refs) {
        buffer_infos.emplace_back(vulkan::DescriptorBufferInfo(ref));
    }
    return buffer_infos;
}
