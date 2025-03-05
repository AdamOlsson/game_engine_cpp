#pragma once

#include "glm/fwd.hpp"
#include "render_engine/CoreGraphicsContext.h"
#include "shape.h"
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
    alignas(4) glm::uint32 shape_type;
    alignas(16) Shape shape;

    StorageBufferObject(glm::vec3 position, glm::vec3 color, glm::float32_t rotation,
                        Shape shape)
        : position(position), color(color), rotation(rotation),
          shape_type(shape.encode_shape_type()), shape(shape) {}
};

class StorageBuffer {
  public:
    std::shared_ptr<CoreGraphicsContext> ctx;
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    void *bufferMapped;
    VkDeviceSize size;

    StorageBuffer(std::shared_ptr<CoreGraphicsContext> ctx, VkBuffer &buffer,
                  VkDeviceMemory &bufferMemory, void *bufferMapped, VkDeviceSize &size)
        : ctx(ctx), buffer(buffer), bufferMemory(bufferMemory),
          bufferMapped(bufferMapped), size(size) {}

    ~StorageBuffer() {
        vkUnmapMemory(ctx->device, bufferMemory);
        vkFreeMemory(ctx->device, bufferMemory, nullptr);
        vkDestroyBuffer(ctx->device, buffer, nullptr);
    }

    void updateStorageBuffer(const std::vector<StorageBufferObject> &ssbo);

    static std::unique_ptr<VkDescriptorSetLayoutBinding>
    createDescriptorSetLayoutBinding(uint32_t binding_num);

    void dumpData();
};

std::unique_ptr<StorageBuffer>
createStorageBuffer(std::shared_ptr<CoreGraphicsContext> &ctx, size_t capacity);
