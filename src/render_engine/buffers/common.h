#pragma once

#include "vulkan/vulkan_core.h"

void createBuffer(const VkPhysicalDevice &physicalDevice, VkDevice &device,
                  const VkDeviceSize size, const VkBufferUsageFlags usage,
                  const VkMemoryPropertyFlags properties, VkBuffer &buffer,
                  VkDeviceMemory &bufferMemory);

uint32_t findMemoryType(const VkPhysicalDevice &physicalDevice, const uint32_t typeFilter,
                        const VkMemoryPropertyFlags properties);

void copyBuffer(const VkDevice &device, const VkBuffer srcBuffer,
                const VkBuffer dstBuffer, const VkDeviceSize size,
                const VkCommandPool &commandPool, const VkQueue &graphicsQueue);
