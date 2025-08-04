#pragma once

#include "render_engine/CommandBufferManager.h"
#include "render_engine/graphics_context/GraphicsContext.h"
#include "vulkan/vulkan_core.h"

void create_buffer(const graphics_context::GraphicsContext *ctx, const VkDeviceSize size,
                   const VkBufferUsageFlags usage, const VkMemoryPropertyFlags properties,
                   VkBuffer &buffer, VkDeviceMemory &buffer_memory);

uint32_t find_memory_type(const graphics_context::GraphicsContext *ctx,
                          const uint32_t type_filter,
                          const VkMemoryPropertyFlags properties);

void copy_buffer(const graphics_context::GraphicsContext *ctx, const VkBuffer src_buffer,
                 const VkBuffer dst_buffer, const VkDeviceSize size,
                 CommandBufferManager *command_buffer_manager);
