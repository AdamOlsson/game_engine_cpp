#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <glm/glm.hpp>
#include <memory>

struct UniformBufferObject {
    alignas(8) glm::vec2 dimensions;
};

class UniformBuffer {
  public:
    std::shared_ptr<CoreGraphicsContext> ctx;

    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    void *bufferMapped;

    VkDeviceSize size;

    UniformBuffer(std::shared_ptr<CoreGraphicsContext> ctx, VkBuffer &buffer,
                  VkDeviceMemory &bufferMemory, void *bufferMapped, VkDeviceSize &size)
        : buffer(buffer), bufferMemory(bufferMemory), bufferMapped(bufferMapped),
          size(size), ctx(ctx) {}

    ~UniformBuffer() {
        vkUnmapMemory(ctx->device, bufferMemory);
        vkFreeMemory(ctx->device, bufferMemory, nullptr);
        vkDestroyBuffer(ctx->device, buffer, nullptr);
    }

    void updateUniformBuffer(const UniformBufferObject &);

    static std::unique_ptr<VkDescriptorSetLayoutBinding>
    createDescriptorSetLayoutBinding(uint32_t binding_num);

    void dumpData();
};

std::unique_ptr<UniformBuffer>
createUniformBuffer(std::shared_ptr<CoreGraphicsContext> &ctx, size_t size);
