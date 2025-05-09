#pragma once

#include "render_engine/SwapChainManager.h"
#include "vulkan/vulkan_core.h"

void create_buffer(const CoreGraphicsContext *ctx, const VkDeviceSize size,
                   const VkBufferUsageFlags usage, const VkMemoryPropertyFlags properties,
                   VkBuffer &buffer, VkDeviceMemory &buffer_memory);

uint32_t find_memory_type(const CoreGraphicsContext *ctx, const uint32_t type_filter,
                          const VkMemoryPropertyFlags properties);

void copy_buffer(const CoreGraphicsContext *ctx, const VkBuffer src_buffer,
                 const VkBuffer dst_buffer, const VkDeviceSize size,
                 SwapChainManager &swap_chain_manager, const VkQueue &graphics_queue);

void copy_buffer_to_image(const VkDevice &device, const VkCommandPool &command_pool,
                          const VkQueue &graphics_queue, VkBuffer &buffer, VkImage &image,
                          const uint32_t width, const uint32_t height);

VkImageView create_image_view(const CoreGraphicsContext *ctx, const VkImage &image,
                              const VkFormat &format);
