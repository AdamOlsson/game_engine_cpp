#pragma once

#include "common.h"
#include "game_engine_sdk/render_engine/vulkan/context/GraphicsContext.h"
#include "vulkan/traits.h"
#include "vulkan/vulkan_core.h"
#include <cstdint>
#include <iostream>
#include <memory>
#include <utility>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

namespace vulkan::buffers {
enum class GpuBufferType { Uniform, Storage };

struct GpuBufferRef {
    VkDeviceSize size;
    VkBuffer buffer;
    GpuBufferType type;
};
template <GpuBufferType BufferType> class BufferDescriptor;

template <Printable T, GpuBufferType BufferType> class GpuBuffer {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;

    std::vector<T> m_staging_buffer;
    size_t m_capacity;

    VkDeviceSize m_size; // number of bytes
    VkBuffer m_buffer;
    VkDeviceMemory m_buffer_memory;
    void *m_buffer_mapped;

    void check_buffer_size() {
        if (m_staging_buffer.size() > m_capacity) {
            logger::warning("Exceeding the GPU buffers size!");
        }
    }

  public:
    GpuBuffer() = default;

    GpuBuffer(std::shared_ptr<vulkan::context::GraphicsContext> ctx, size_t capacity)
        : m_ctx(ctx), m_capacity(capacity), m_size(capacity * sizeof(T)) {

        if constexpr (BufferType == GpuBufferType::Storage) {
            create_buffer(m_ctx.get(), m_size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                          m_buffer, m_buffer_memory);

        } else if constexpr (BufferType == GpuBufferType::Uniform) {
            create_buffer(m_ctx.get(), m_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                          m_buffer, m_buffer_memory);
        }

        m_staging_buffer.reserve(capacity);

        vkMapMemory(m_ctx->logical_device, m_buffer_memory, 0, m_size, 0,
                    &m_buffer_mapped);
        memset(m_buffer_mapped, 0, m_size);
    }

    ~GpuBuffer() {
        if (m_buffer == VK_NULL_HANDLE) {
            return;
        }
        vkUnmapMemory(m_ctx->logical_device, m_buffer_memory);
        vkFreeMemory(m_ctx->logical_device, m_buffer_memory, nullptr);
        vkDestroyBuffer(m_ctx->logical_device, m_buffer, nullptr);
    }

    GpuBuffer(const GpuBuffer &other) = delete;
    GpuBuffer &operator=(const GpuBuffer &other) = delete;

    GpuBuffer(GpuBuffer &&other) noexcept
        : m_ctx(std::move(other.m_ctx)), m_size(std::move(other.m_size)),
          m_staging_buffer(std::move(other.m_staging_buffer)),
          m_buffer(std::move(other.m_buffer)),
          m_buffer_memory(std::move(other.m_buffer_memory)),
          m_buffer_mapped(std::move(other.m_buffer_mapped)) {
        other.m_buffer = VK_NULL_HANDLE;
        other.m_buffer_memory = VK_NULL_HANDLE;
        other.m_buffer_mapped = VK_NULL_HANDLE;
    }

    GpuBuffer &operator=(GpuBuffer &&other) noexcept {
        if (this != &other) {
            m_ctx = std::move(other.m_ctx);
            m_size = std::move(other.m_size);
            m_staging_buffer = std::move(other.m_staging_buffer);
            m_buffer = std::move(other.m_buffer);
            m_buffer_memory = std::move(other.m_buffer_memory);
            m_buffer_mapped = std::move(other.m_buffer_mapped);

            other.m_buffer = VK_NULL_HANDLE;
            other.m_buffer_memory = VK_NULL_HANDLE;
            other.m_buffer_mapped = VK_NULL_HANDLE;
        }
        return *this;
    }

    GpuBufferRef get_reference() {
        return {.size = m_size, .buffer = m_buffer, .type = BufferType};
    }
    size_t size() const { return m_size; }
    size_t num_elements() const { return m_staging_buffer.size(); }
    void clear() { m_staging_buffer.clear(); }
    void transfer() { memcpy(m_buffer_mapped, m_staging_buffer.data(), m_size); }

    void push_back(T &t) {
        check_buffer_size();
        return m_staging_buffer.push_back(t);
    }
    void push_back(const T &t) {
        check_buffer_size();
        return m_staging_buffer.push_back(t);
    }

    decltype(auto) push_back(T &&t) {
        check_buffer_size();
        return m_staging_buffer.push_back(std::forward<T>(t));
    }

    template <typename... Args> decltype(auto) emplace_back(Args &&...args) {
        check_buffer_size();
        return m_staging_buffer.emplace_back(std::forward<Args>(args)...);
    }

    T &operator[](size_t index) { return m_staging_buffer[index]; }
    const T &operator[](size_t index) const { return m_staging_buffer[index]; }

    VkDescriptorSetLayoutBinding
    create_descriptor_set_layout_binding(uint32_t binding_num) {
        return BufferDescriptor<BufferType>::create_descriptor_set_layout_binding(
            binding_num);
    }

    void dump_data() const {
        std::cout << "=== GpuBuffer Data Dump ===" << std::endl;
        if constexpr (BufferType == GpuBufferType::Storage) {
            std::cout << "Buffer type: " << "Storage" << std::endl;
        } else if constexpr (BufferType == GpuBufferType::Uniform) {
            std::cout << "Buffer type: " << "Uniform" << std::endl;
        }
        std::cout << "Buffer size: " << m_size << " bytes" << std::endl;
        std::cout << "Buffer capacity: " << m_staging_buffer.capacity() << std::endl;
        std::cout << "Staging buffer size: " << m_staging_buffer.size() << " elements"
                  << std::endl;

        // Print staging buffer contents
        std::cout << "\nStaging buffer contents:" << std::endl;
        for (size_t i = 0; i < m_staging_buffer.size(); ++i) {
            std::cout << "[" << i << "]: " << m_staging_buffer[i] << std::endl;
        }

        // Print GPU buffer contents (if data has been transferred)
        if (m_buffer_mapped != nullptr) {
            std::cout << "\nGPU buffer contents:" << std::endl;
            auto *mapped_data = static_cast<T *>(m_buffer_mapped);
            /*size_t element_count = m_size / sizeof(T);*/
            size_t element_count = m_staging_buffer.size();

            for (size_t i = 0; i < element_count; ++i) {
                std::cout << "[" << i << "]: " << mapped_data[i] << std::endl;
            }

            // Also show raw byte data for debugging
            std::cout << "\nRaw byte data (first "
                      << std::min(m_size, static_cast<VkDeviceSize>(64))
                      << " bytes):" << std::endl;
            unsigned char *byteData = static_cast<unsigned char *>(m_buffer_mapped);
            for (size_t i = 0; i < std::min(m_size, static_cast<VkDeviceSize>(64)); i++) {
                printf("%02x ", byteData[i]);
                if ((i + 1) % 16 == 0)
                    printf("\n");
            }
            if (m_size % 16 != 0) {
                printf("\n");
            }
        }

        std::cout << "=== End Data Dump ===" << std::endl;
    }
};

template <typename T> using StorageBuffer = GpuBuffer<T, GpuBufferType::Storage>;
template <typename T> using UniformBuffer = GpuBuffer<T, GpuBufferType::Uniform>;

template <GpuBufferType BufferType> class BufferDescriptor {
  public:
    static VkDescriptorSetLayoutBinding
    create_descriptor_set_layout_binding(uint32_t binding_num) {
        VkDescriptorSetLayoutBinding layout_binding{};
        layout_binding.binding = binding_num;
        if constexpr (BufferType == GpuBufferType::Storage) {
            layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        } else if constexpr (BufferType == GpuBufferType::Uniform) {
            layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
        layout_binding.descriptorCount = 1;
        layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        layout_binding.pImmutableSamplers = nullptr;
        return layout_binding;
    }
};

template <typename T, GpuBufferType BufferType> class SwapGpuBuffer {
  private:
    size_t m_idx;
    std::vector<GpuBuffer<T, BufferType>> m_buffers;
    std::vector<GpuBufferRef> m_refs;

  public:
    SwapGpuBuffer() = default;

    SwapGpuBuffer(std::shared_ptr<vulkan::context::GraphicsContext> &ctx, size_t num_bufs,
                  size_t capacity)
        : m_idx(0) {
        // Initiate buffers
        m_buffers.reserve(num_bufs);
        for (auto i = 0; i < num_bufs; i++) {
            m_buffers.emplace_back(ctx, capacity);
        }

        // Create buffer references
        m_refs.reserve(num_bufs);
        for (auto i = 0; i < m_buffers.size(); i++) {
            m_refs.push_back(m_buffers[i].get_reference());
        }
    }

    GpuBuffer<T, BufferType> &get_buffer() { return m_buffers[m_idx]; }

    void rotate() { m_idx = ++m_idx % m_buffers.size(); }

    void write(T &val) {
        for (auto &buf : m_buffers) {
            buf.push_back(val);
            buf.transfer();
        }
    }

    void write(T &&val) {
        for (auto &buf : m_buffers) {
            buf.push_back(val);
            buf.transfer();
        }
    }

    void write(const T &val) {
        for (auto &buf : m_buffers) {
            buf.push_back(val);
            buf.transfer();
        }
    }

    void dump_data() {
        for (auto &buf : m_buffers) {
            buf.dump_data();
        }
    }

    std::vector<GpuBufferRef> get_buffer_references() { return m_refs; }
};

template <typename T> using SwapStorageBuffer = SwapGpuBuffer<T, GpuBufferType::Storage>;
template <typename T> using SwapUniformBuffer = SwapGpuBuffer<T, GpuBufferType::Uniform>;
} // namespace vulkan::buffers
