#pragma once

#include "vulkan/vulkan_core.h"

void createBuffer(VkPhysicalDevice &physicalDevice, VkDevice &device, VkDeviceSize size,
                  VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                  VkBuffer &buffer, VkDeviceMemory &bufferMemory);

uint32_t findMemoryType(VkPhysicalDevice &physicalDevice, uint32_t typeFilter,
                        VkMemoryPropertyFlags properties);

void copyBuffer(VkDevice &device, VkBuffer srcBuffer, VkBuffer dstBuffer,
                VkDeviceSize size, VkCommandPool &commandPool, VkQueue &graphicsQueue);
