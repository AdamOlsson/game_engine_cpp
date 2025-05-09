#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <glm/glm.hpp>
#include <memory>

struct UniformBufferObject {
    alignas(8) glm::vec2 dimensions;
};

class UniformBuffer {
  private:
    std::shared_ptr<CoreGraphicsContext> m_ctx;

  public:
    VkBuffer buffer;
    VkDeviceMemory buffer_memory;
    void *buffer_mapped;
    VkDeviceSize size;

    UniformBuffer();
    UniformBuffer(std::shared_ptr<CoreGraphicsContext> ctx, const size_t size);

    UniformBuffer(UniformBuffer &&) noexcept;            // Move constructor
    UniformBuffer &operator=(UniformBuffer &&) noexcept; // Move assignment

    ~UniformBuffer();

    UniformBuffer(const UniformBuffer &) = delete;            // Copy constructor
    UniformBuffer &operator=(const UniformBuffer &) = delete; // Copy assignment

    void update_uniform_buffer(const UniformBufferObject &);

    static VkDescriptorSetLayoutBinding
    create_descriptor_set_layout_binding(uint32_t binding_num);

    void dump_data();
};
