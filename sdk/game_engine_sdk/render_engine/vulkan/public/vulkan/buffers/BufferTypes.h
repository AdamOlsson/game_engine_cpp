#pragma once

#include "vulkan/vulkan_core.h"

namespace vulkan::buffers {
enum class GpuBufferType { Uniform, Storage, Indirect };

struct GpuBufferRef {
    VkDeviceSize size;
    VkBuffer buffer;
    GpuBufferType type;
};

template <GpuBufferType BufferType> class BufferDescriptor2 {
  public:
    static VkDescriptorSetLayoutBinding
    create_descriptor_set_layout_binding(uint32_t binding_num) {
        VkDescriptorSetLayoutBinding layout_binding{};
        layout_binding.binding = binding_num;
        if constexpr (BufferType == GpuBufferType::Storage) {
            layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        } else if constexpr (BufferType == GpuBufferType::Uniform) {
            layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
        layout_binding.descriptorCount = 1;
        layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        layout_binding.pImmutableSamplers = nullptr;
        return layout_binding;
    }
};

} // namespace vulkan::buffers
