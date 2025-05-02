#include "StorageBuffer.h"
#include "render_engine/buffers/common.h"
#include "vulkan/vulkan_core.h"
#include <iostream>
#include <ostream>

StorageBuffer::StorageBuffer()
    : ctx(nullptr), size(0), buffer(VK_NULL_HANDLE), bufferMemory(VK_NULL_HANDLE),
      bufferMapped(nullptr) {}

StorageBuffer::StorageBuffer(std::shared_ptr<CoreGraphicsContext> ctx, size_t capacity)
    : ctx(ctx), size(capacity * sizeof(StorageBufferObject)) {

    createBuffer(
        ctx->physicalDevice, ctx->device, size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        buffer, bufferMemory);

    vkMapMemory(ctx->device, bufferMemory, 0, size, 0, &bufferMapped);
}

StorageBuffer::~StorageBuffer() {
    if (buffer == VK_NULL_HANDLE) {
        return;
    }
    vkUnmapMemory(ctx->device, bufferMemory);
    vkFreeMemory(ctx->device, bufferMemory, nullptr);
    vkDestroyBuffer(ctx->device, buffer, nullptr);
}

// Move constructor
StorageBuffer::StorageBuffer(StorageBuffer &&other) noexcept
    : ctx(std::move(other.ctx)), buffer(other.buffer), bufferMemory(other.bufferMemory),
      bufferMapped(other.bufferMapped), size(other.size) {
    other.buffer = VK_NULL_HANDLE;
    other.bufferMemory = VK_NULL_HANDLE;
    other.bufferMapped = nullptr;
    other.size = 0;
}

// Move assignement
StorageBuffer &StorageBuffer::operator=(StorageBuffer &&other) noexcept {
    if (this != &other) {
        ctx = std::move(other.ctx);
        buffer = other.buffer;
        bufferMemory = other.bufferMemory;
        bufferMapped = other.bufferMapped;
        size = other.size;

        other.buffer = VK_NULL_HANDLE;
        other.bufferMemory = VK_NULL_HANDLE;
        other.bufferMapped = nullptr;
        other.size = 0;
    }

    return *this;
}

VkDescriptorSetLayoutBinding
StorageBuffer::create_descriptor_set_layout_binding(uint32_t binding_num) {
    VkDescriptorSetLayoutBinding layout_binding{};
    layout_binding.binding = binding_num;
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    layout_binding.descriptorCount = 1;
    layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layout_binding.pImmutableSamplers = nullptr; // Optional
    return layout_binding;
}

void StorageBuffer::update_storage_buffer(const std::vector<StorageBufferObject> &ssbo) {
    const size_t write_size = sizeof(ssbo[0]) * ssbo.size();
    const size_t diff = size - write_size;
    memcpy(bufferMapped, &ssbo[0], write_size);
}

void StorageBuffer::dump_data() {
    // Dump the data to verify
    auto *mappedData = reinterpret_cast<StorageBufferObject *>(bufferMapped);
    std::cout << "SSBO Data Dump:" << std::endl;
    std::cout << "Size of SSBO: " << sizeof(StorageBufferObject) << std::endl;
    std::cout << "Position: " << mappedData->position.x << ", " << mappedData->position.y
              << ", " << mappedData->position.z << std::endl;
    std::cout << "Color: " << mappedData->color.x << ", " << mappedData->color.y << ", "
              << mappedData->color.z << std::endl;
    std::cout << "Rotation: " << mappedData->rotation << std::endl;
    /*std::cout << "Shape: " << mappedData->rotation << std::endl;*/

    unsigned char *byteData = reinterpret_cast<unsigned char *>(bufferMapped);
    for (size_t i = 0; i < sizeof(StorageBufferObject); i++) {
        printf("%02x ", byteData[i]);
    }
    printf("\n");
}
