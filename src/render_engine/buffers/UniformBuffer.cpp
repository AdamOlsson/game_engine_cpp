#include "UniformBuffer.h"
#include "render_engine/buffers/common.h"
#include "vulkan/vulkan_core.h"
#include <iostream>

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

UniformBuffer::UniformBuffer()
    : ctx(nullptr), buffer(VK_NULL_HANDLE), buffer_memory(VK_NULL_HANDLE),
      buffer_mapped(nullptr), size(0) {}

UniformBuffer::UniformBuffer(std::shared_ptr<CoreGraphicsContext> ctx, const size_t size)
    : ctx(ctx), size(size) {

    createBuffer(
        ctx->physicalDevice, ctx->device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        buffer, buffer_memory);

    vkMapMemory(ctx->device, buffer_memory, 0, size, 0, &buffer_mapped);
}

UniformBuffer::UniformBuffer(UniformBuffer &&other) noexcept
    : ctx(std::move(other.ctx)), buffer(other.buffer), buffer_memory(other.buffer_memory),
      buffer_mapped(other.buffer_memory), size(other.size) {
    other.buffer = VK_NULL_HANDLE;
    other.buffer_memory = VK_NULL_HANDLE;
    other.buffer_mapped = nullptr;
    other.size = 0;
}

UniformBuffer &UniformBuffer::operator=(UniformBuffer &&other) noexcept {
    if (this != &other) {
        ctx = std::move(other.ctx);
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
    vkUnmapMemory(ctx->device, buffer_memory);
    vkFreeMemory(ctx->device, buffer_memory, nullptr);
    vkDestroyBuffer(ctx->device, buffer, nullptr);
}

void UniformBuffer::updateUniformBuffer(const UniformBufferObject &ssbo) {
    memcpy(buffer_mapped, &ssbo, sizeof(ssbo));
}

VkDescriptorSetLayoutBinding
UniformBuffer::createDescriptorSetLayoutBinding(uint32_t binding_num) {
    VkDescriptorSetLayoutBinding layout_binding{};
    layout_binding.binding = binding_num;
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layout_binding.descriptorCount = 1;
    layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layout_binding.pImmutableSamplers = nullptr; // Optional
    return layout_binding;
}

void UniformBuffer::dumpData() {
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
