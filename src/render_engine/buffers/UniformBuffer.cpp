#include "UniformBuffer.h"
#include "render_engine/buffers/common.h"
#include <iostream>

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

std::unique_ptr<UniformBuffer> createUniformBuffer(VkPhysicalDevice &physicalDevice,
                                                   VkDevice &device, size_t size) {
    VkDeviceSize bufferSize = size;

    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    void *bufferMapped;
    createBuffer(physicalDevice, device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 buffer, bufferMemory);

    vkMapMemory(device, bufferMemory, 0, bufferSize, 0, &bufferMapped);

    return std::make_unique<UniformBuffer>(buffer, bufferMemory, bufferMapped,
                                           bufferSize);
}

void UniformBuffer::updateUniformBuffer(const UniformBufferObject &ssbo) {
    memcpy(bufferMapped, &ssbo, sizeof(ssbo));
}

std::unique_ptr<VkDescriptorSetLayoutBinding>
UniformBuffer::createDescriptorSetLayoutBinding(uint32_t binding_num) {
    auto layoutBinding = std::make_unique<VkDescriptorSetLayoutBinding>();
    layoutBinding->binding = binding_num;
    layoutBinding->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBinding->descriptorCount = 1;
    layoutBinding->stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBinding->pImmutableSamplers = nullptr; // Optional
    return layoutBinding;
}

void UniformBuffer::dumpData() {
    auto *mappedData = reinterpret_cast<UniformBufferObject *>(bufferMapped);
    std::cout << "Uniform Buffer Data Dump:" << std::endl;
    std::cout << "Size of UniformBufferObject: " << sizeof(UniformBufferObject)
              << std::endl;
    std::cout << "Dimensions: (" << mappedData->dimensions.x << ", "
              << mappedData->dimensions.y << ")" << std::endl;

    unsigned char *byteData = reinterpret_cast<unsigned char *>(bufferMapped);
    for (size_t i = 0; i < sizeof(UniformBufferObject); i++) {
        printf("%02x ", byteData[i]);
    }
    printf("\n");
}
