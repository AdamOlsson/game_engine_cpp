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
    alignas(16) glm::vec4 uvwt;

    StorageBufferObject(glm::vec3 position, glm::vec3 color, glm::float32_t rotation,
                        Shape shape, glm::vec4 uvwt)
        : position(position), color(color), rotation(rotation),
          shape_type(shape.encode_shape_type()), shape(shape), uvwt(uvwt) {}
};

class StorageBuffer {
  private:
    std::shared_ptr<CoreGraphicsContext> ctx;

  public:
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    void *bufferMapped;
    VkDeviceSize size;

    StorageBuffer();
    StorageBuffer(std::shared_ptr<CoreGraphicsContext> ctx, size_t capacity);

    /* Example:
     *  StorageBuffer a;
     *  StorageBuffer b = std::move(a);
     */
    StorageBuffer(StorageBuffer &&other) noexcept; // Move constructor

    /* Example:
     *  StorageBuffer a;
     *  StorageBuffer b;
     *  b = std::move(a);
     */
    StorageBuffer &operator=(StorageBuffer &&other) noexcept; // Move assignment

    /* Example:
     *  StorageBuffer a;
     *  StorageBuffer b = a;
     */
    StorageBuffer(const StorageBuffer &other) = delete; // Copy constructor

    /* Example:
     *  StorageBuffer a;
     *  StorageBuffer b;
     *  b = a;
     */
    StorageBuffer &operator=(const StorageBuffer &other) = delete; // Copy assignment

    ~StorageBuffer();

    void updateStorageBuffer(const std::vector<StorageBufferObject> &ssbo);

    static VkDescriptorSetLayoutBinding
    createDescriptorSetLayoutBinding(uint32_t binding_num);

    void dumpData();
};
