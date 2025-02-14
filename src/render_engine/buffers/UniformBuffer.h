#pragma once

#include "vulkan/vulkan_core.h"
#include <glm/glm.hpp>
#include <memory>

struct UniformBufferObject {
    alignas(8) glm::vec2 dimensions;
};

struct UniformBuffer {
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    void *bufferMapped;

    VkDeviceSize size;

    UniformBuffer(VkBuffer &buffer, VkDeviceMemory &bufferMemory, void *bufferMapped,
                  VkDeviceSize &size)
        : buffer(buffer), bufferMemory(bufferMemory), bufferMapped(bufferMapped),
          size(size) {}

    void updateUniformBuffer(const UniformBufferObject &);

    static std::unique_ptr<VkDescriptorSetLayoutBinding>
    createDescriptorSetLayoutBinding(uint32_t binding_num);

    void dumpData();
};

std::unique_ptr<UniformBuffer> createUniformBuffer(VkPhysicalDevice &physicalDevice,
                                                   VkDevice &device, size_t size);
