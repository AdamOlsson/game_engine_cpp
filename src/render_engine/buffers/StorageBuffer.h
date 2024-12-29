#pragma once

#include "vulkan/vulkan_core.h"
#include <cstdint>
#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

struct StorageBuffer {
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    void *bufferMapped;

    StorageBuffer(VkBuffer &buffer, VkDeviceMemory &bufferMemory, void *bufferMapped)
        : buffer(buffer), bufferMemory(bufferMemory), bufferMapped(bufferMapped) {}

    void updateStorageBuffer();

    static VkDescriptorSetLayout createDescriptorSetLayout(VkDevice &device,
                                                           uint32_t binding_num);

    void dumpData();
};

struct StorageBufferObject {
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 color;
    glm::float32_t rotation;
};

std::unique_ptr<StorageBuffer> createStorageBuffer(VkPhysicalDevice &physicalDevice,
                                                   VkDevice &device);
