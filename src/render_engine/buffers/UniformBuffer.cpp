#include "UniformBuffer.h"
#include "render_engine/buffers/common.h"
#include "vulkan/vulkan_core.h"
#include <iostream>

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

UniformBuffer::UniformBuffer()
    : m_ctx(nullptr), buffer(VK_NULL_HANDLE), buffer_memory(VK_NULL_HANDLE),
      buffer_mapped(nullptr), size(0) {}

UniformBuffer::UniformBuffer(std::shared_ptr<CoreGraphicsContext> ctx, const size_t size)
    : m_ctx(ctx), size(size) {

    create_buffer(m_ctx.get(), size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                  buffer, buffer_memory);

    vkMapMemory(m_ctx->device, buffer_memory, 0, size, 0, &buffer_mapped);
}

UniformBuffer::UniformBuffer(UniformBuffer &&other) noexcept
    : m_ctx(std::move(other.m_ctx)), buffer(other.buffer),
      buffer_memory(other.buffer_memory), buffer_mapped(other.buffer_memory),
      size(other.size) {
    other.buffer = VK_NULL_HANDLE;
    other.buffer_memory = VK_NULL_HANDLE;
    other.buffer_mapped = nullptr;
    other.size = 0;
}

UniformBuffer &UniformBuffer::operator=(UniformBuffer &&other) noexcept {
    if (this != &other) {
        m_ctx = std::move(other.m_ctx);
        buffer = other.buffer;
        buffer_memory = other.buffer_memory;
        buffer_mapped = other.buffer_mapped;
        size = other.size;

        other.buffer = VK_NULL_HANDLE;
        other.buffer_memory = VK_NULL_HANDLE;
        other.buffer_mapped = nullptr;
        other.size = 0;
    }
    return *this;
}

UniformBuffer::~UniformBuffer() {
    if (buffer == VK_NULL_HANDLE) {
        return;
    }
    vkUnmapMemory(m_ctx->device, buffer_memory);
    vkFreeMemory(m_ctx->device, buffer_memory, nullptr);
    vkDestroyBuffer(m_ctx->device, buffer, nullptr);
}

void UniformBuffer::update_uniform_buffer(const UniformBufferObject &ssbo) {
    memcpy(buffer_mapped, &ssbo, sizeof(ssbo));
}

VkDescriptorSetLayoutBinding
UniformBuffer::create_descriptor_set_layout_binding(uint32_t binding_num) {
    VkDescriptorSetLayoutBinding layout_binding{};
    layout_binding.binding = binding_num;
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layout_binding.descriptorCount = 1;
    layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layout_binding.pImmutableSamplers = nullptr; // Optional
    return layout_binding;
}

void UniformBuffer::dump_data() {
    auto *mappedData = reinterpret_cast<UniformBufferObject *>(buffer_mapped);
    std::cout << "Uniform Buffer Data Dump:" << std::endl;
    std::cout << "Size of UniformBufferObject: " << sizeof(UniformBufferObject)
              << std::endl;
    std::cout << "Dimensions: (" << mappedData->dimensions.x << ", "
              << mappedData->dimensions.y << ")" << std::endl;

    unsigned char *byteData = reinterpret_cast<unsigned char *>(buffer_mapped);
    for (size_t i = 0; i < sizeof(UniformBufferObject); i++) {
        printf("%02x ", byteData[i]);
    }
    printf("\n");
}
