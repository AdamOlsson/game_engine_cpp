#include "StorageBuffer.h"
#include "render_engine/buffers/common.h"
#include "vulkan/vulkan_core.h"
#include <iostream>
#include <ostream>

std::unique_ptr<StorageBuffer>
createStorageBuffer(std::shared_ptr<CoreGraphicsContext> &ctx, size_t capacity) {

    VkDeviceSize bufferSize = capacity * sizeof(StorageBufferObject);

    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    void *bufferMapped;
    createBuffer(
        ctx->physicalDevice, ctx->device, bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        buffer, bufferMemory);

    vkMapMemory(ctx->device, bufferMemory, 0, bufferSize, 0, &bufferMapped);

    return std::make_unique<StorageBuffer>(ctx, buffer, bufferMemory, bufferMapped,
                                           bufferSize);
}

std::unique_ptr<VkDescriptorSetLayoutBinding>
StorageBuffer::createDescriptorSetLayoutBinding(uint32_t binding_num) {
    auto layoutBinding = std::make_unique<VkDescriptorSetLayoutBinding>();
    layoutBinding->binding = binding_num;
    layoutBinding->descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    layoutBinding->descriptorCount = 1;
    layoutBinding->stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBinding->pImmutableSamplers = nullptr; // Optional

    return layoutBinding;
}

void StorageBuffer::updateStorageBuffer(const std::vector<StorageBufferObject> &ssbo) {
    const size_t write_size = sizeof(ssbo[0]) * ssbo.size();
    const size_t diff = size - write_size;
    memcpy(bufferMapped, &ssbo[0], write_size);
    /*memset((char *)bufferMapped + write_size, 0, diff);*/
}

void StorageBuffer::dumpData() {
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
