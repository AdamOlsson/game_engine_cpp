#pragma once

#include "util/assert.h"
#include "vulkan/buffers/BufferTypes.h"
#include "vulkan/buffers/common.h"
#include "vulkan/context/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <memory>

namespace vulkan::buffers {

template <GpuBufferType BufferType> class BufferDescriptor;

template <typename T, GpuBufferType BufferType> class GpuBuffer {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;

    size_t m_capacity;   // number of elements
    VkDeviceSize m_size; // number of bytes

    struct Buffer {
        VkBuffer buffer_handle;
        VkDeviceMemory buffer_memory;
        void *buffer_mapped;
    };

    std::vector<Buffer> m_buffers;
    std::vector<GpuBufferRef> m_refs;

    struct {
        size_t swap_size;
        size_t current_idx = 0;
        void rotate() { current_idx = ++current_idx % swap_size; }
        size_t current() { return current_idx; }
        size_t next() { return ++current_idx % swap_size; }
    } m_swap_state;

  public:
    GpuBuffer() = default;

    GpuBuffer(std::shared_ptr<vulkan::context::GraphicsContext> ctx,
              const size_t capacity, const size_t swap_size = 1,
              const std::optional<VkBufferUsageFlagBits> usage = std::nullopt)
        : m_ctx(ctx), m_capacity(capacity), m_size(capacity * sizeof(T)),
          m_swap_state({.swap_size = swap_size}) {

        if (m_swap_state.swap_size < 1) {
            throw std::runtime_error("Error: swap_size needs to larger than 0.");
        }

        VkBufferUsageFlagBits usage_;
        if constexpr (BufferType == GpuBufferType::Storage) {
            usage_ = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        } else if constexpr (BufferType == GpuBufferType::Uniform) {
            usage_ = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        } else if constexpr (BufferType == GpuBufferType::Indirect) {
            usage_ = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        }

        if (usage.has_value()) {
            usage_ = static_cast<VkBufferUsageFlagBits>(usage_ | usage.value());
        }

        m_buffers.resize(m_swap_state.swap_size);
        m_refs.reserve(m_swap_state.swap_size);
        for (Buffer &b : m_buffers) {
            create_buffer(m_ctx.get(), m_size, usage_,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                          b.buffer_handle, b.buffer_memory);

            vkMapMemory(m_ctx->logical_device, b.buffer_memory, 0, m_size, 0,
                        &b.buffer_mapped);
            memset(b.buffer_mapped, 0, m_size);

            m_refs.push_back(GpuBufferRef{
                .size = m_size, .buffer = b.buffer_handle, .type = BufferType});
        }
    }

    ~GpuBuffer() {
        for (Buffer &b : m_buffers) {
            if (b.buffer_handle == VK_NULL_HANDLE) {
                continue;
            }
            vkUnmapMemory(m_ctx->logical_device, b.buffer_memory);
            vkFreeMemory(m_ctx->logical_device, b.buffer_memory, nullptr);
            vkDestroyBuffer(m_ctx->logical_device, b.buffer_handle, nullptr);
        }
    }

    GpuBuffer(const GpuBuffer &other) = delete;
    GpuBuffer &operator=(const GpuBuffer &other) = delete;

    GpuBuffer(GpuBuffer &&other) noexcept
        : m_ctx(std::move(other.m_ctx)), m_size(std::move(other.m_size)),
          m_buffers(std::move(other.m_buffers)), m_capacity(other.m_capacity),
          m_swap_state(other.m_swap_state), m_refs(std::move(other.m_refs)) {
        other.m_buffers.clear();
        other.m_capacity = 0;
    }

    GpuBuffer &operator=(GpuBuffer &&other) noexcept {
        if (this != &other) {
            m_ctx = std::move(other.m_ctx);
            m_size = std::move(other.m_size);
            m_buffers = std::move(other.m_buffers);
            m_swap_state = std::move(other.m_swap_state);
            m_refs = std::move(other.m_refs);
            m_capacity = other.m_capacity;

            other.m_capacity = 0;
        }
        return *this;
    }

    VkBuffer handle() { return m_buffers[m_swap_state.current()].buffer_handle; }

    void write_indices(const std::vector<T> &data, const std::vector<size_t> &indices,
                       const size_t offset = 0) {
        const size_t current_buffer = m_swap_state.current();
        auto *base = static_cast<std::byte *>(m_buffers[current_buffer].buffer_mapped);
        DEBUG_ASSERT(offset + indices.size() <= m_capacity,
                     "Error: Write would exceed GPU buffer capacity!");
        for (size_t i : indices) {
            std::memcpy(base + offset * sizeof(T), &data[i], sizeof(T));
        }
    }

    void write(const std::vector<T> &data, const size_t offset = 0) {
        const size_t current_buffer = m_swap_state.current();
        auto *base = static_cast<std::byte *>(m_buffers[current_buffer].buffer_mapped);
        const size_t num_elements = offset + data.size();
        DEBUG_ASSERT(num_elements <= m_capacity,
                     "Error: Write would exceed GPU buffer capacity!");
        std::memcpy(base + offset * sizeof(T), data.data(), sizeof(T) * data.size());
    }

    size_t size() const { return m_size; }

    std::vector<GpuBufferRef> get_reference() { return m_refs; }

    void rotate() { m_swap_state.rotate(); }

    VkDescriptorSetLayoutBinding
    create_descriptor_set_layout_binding(uint32_t binding_num) {
        return BufferDescriptor<BufferType>::create_descriptor_set_layout_binding(
            binding_num);
    }
};

template <typename T> using StorageBuffer = GpuBuffer<T, GpuBufferType::Storage>;
template <typename T> using UniformBuffer = GpuBuffer<T, GpuBufferType::Uniform>;
template <typename T> using IndirectBuffer = GpuBuffer<T, GpuBufferType::Indirect>;

} // namespace vulkan::buffers
