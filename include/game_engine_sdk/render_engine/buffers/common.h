#pragma once

#include "game_engine_sdk/render_engine/vulkan/GraphicsContext.h"
#include "vulkan/vulkan_core.h"

void create_buffer(const vulkan::GraphicsContext *ctx, const VkDeviceSize size,
                   const VkBufferUsageFlags usage, const VkMemoryPropertyFlags properties,
                   VkBuffer &buffer, VkDeviceMemory &buffer_memory);
