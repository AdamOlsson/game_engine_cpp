#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/buffers/StorageBuffer.h"
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

void copy_buffer_to_image(const VkDevice &device, const VkCommandPool &command_pool,
                          const VkQueue &graphics_queue, VkBuffer &buffer, VkImage &image,
                          const uint32_t width, const uint32_t height);

VkImageView create_image_view(const VkDevice &device, const VkImage &image,
                              const VkFormat &format);

std::vector<StorageBuffer>
create_instance_buffers(std::shared_ptr<CoreGraphicsContext> &ctx, const size_t capacity);

VkCommandBuffer begin_single_time_commands(const VkDevice &device,
                                           const VkCommandPool &commandPool);

void end_single_time_commands(const VkDevice &device, const VkCommandPool &command_pool,
                              const VkCommandBuffer command_buffer,
                              const VkQueue &graphics_queue);
