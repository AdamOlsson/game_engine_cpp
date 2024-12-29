#pragma once

#include "vulkan/vulkan_core.h"
#include <glm/glm.hpp>
#include <memory>

struct UniformBuffer {
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    void *bufferMapped;

    UniformBuffer(VkBuffer &buffer, VkDeviceMemory &bufferMemory, void *bufferMapped)
        : buffer(buffer), bufferMemory(bufferMemory), bufferMapped(bufferMapped) {}

    void updateUniformBuffer(uint32_t currentImage);

    static VkDescriptorSetLayout createDescriptorSetLayout(VkDevice &device,
                                                           uint32_t binding_num);
};

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

std::unique_ptr<UniformBuffer> createUniformBuffer(VkPhysicalDevice &physicalDevice,
                                                   VkDevice &device);
