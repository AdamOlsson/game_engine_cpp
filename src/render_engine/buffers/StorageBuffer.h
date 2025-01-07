#pragma once

#include "glm/glm.hpp"
#include "vulkan/vulkan_core.h"
#include <cstdint>
#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

struct StorageBufferObject {
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 color;
    alignas(4) glm::float32_t rotation;

    // Shape specific data
    alignas(4) glm::float32_t side;

    StorageBufferObject(glm::vec3 position, glm::vec3 color, glm::float32_t rotation,
                        glm::float32_t side)
        : position(position), color(color), rotation(rotation), side(side) {}
};

struct StorageBuffer {
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    void *bufferMapped;

    VkDeviceSize size;

    StorageBuffer(VkBuffer &buffer, VkDeviceMemory &bufferMemory, void *bufferMapped,
                  VkDeviceSize &size)
        : buffer(buffer), bufferMemory(bufferMemory), bufferMapped(bufferMapped),
          size(size) {}

    void updateStorageBuffer(const std::vector<StorageBufferObject> &ssbo);

    static std::unique_ptr<VkDescriptorSetLayoutBinding>
    createDescriptorSetLayoutBinding(uint32_t binding_num);

    void dumpData();
};

std::unique_ptr<StorageBuffer> createStorageBuffer(VkPhysicalDevice &physicalDevice,
                                                   VkDevice &device, size_t capacity);
