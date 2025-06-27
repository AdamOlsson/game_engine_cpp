#pragma once

#include "glm/fwd.hpp"
#include "render_engine/CoreGraphicsContext.h"
#include "shape.h"
#include "vulkan/vulkan_core.h"
#include <cstdint>
#include <iostream>
#include <memory>
#include <utility>

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

    std::string to_string() const {
        return std::format("StorageBufferObject {{\n"
                           "  position:   ({:.3f}, {:.3f}, {:.3f})\n"
                           "  color:      ({:.3f}, {:.3f}, {:.3f})\n"
                           "  rotation:   {:.3f}°\n"
                           "  shape_type: {}\n"
                           "  shape:      {}\n"
                           "  uvwt:       ({:.3f}, {:.3f}, {:.3f}, {:.3f})\n"
                           "}}",
                           position.x, position.y, position.z, color.r, color.g, color.b,
                           rotation, shape_type,
                           shape.to_string(), // Assuming Shape has a to_string() method
                           uvwt.x, uvwt.y, uvwt.z, uvwt.w);
    }

    friend std::ostream &operator<<(std::ostream &os, const StorageBufferObject &obj) {
        return os << obj.to_string();
    }
};

class StorageBuffer {
  private:
    std::shared_ptr<CoreGraphicsContext> m_ctx;

    VkDeviceMemory bufferMemory;
    void *bufferMapped;

  public:
    VkBuffer buffer;
    VkDeviceSize size;

    StorageBuffer();
    StorageBuffer(std::shared_ptr<CoreGraphicsContext> ctx, size_t capacity);
    ~StorageBuffer();

    StorageBuffer(StorageBuffer &&other) noexcept;
    StorageBuffer(const StorageBuffer &other) = delete;

    StorageBuffer &operator=(StorageBuffer &&other) noexcept;
    StorageBuffer &operator=(const StorageBuffer &other) = delete;

    void update_storage_buffer(const std::vector<StorageBufferObject> &ssbo);

    static VkDescriptorSetLayoutBinding
    create_descriptor_set_layout_binding(uint32_t binding_num);

    void dump_data();
};

template <typename T>
concept Printable = requires(T t) { std::cout << t; };

template <Printable T> class StorageBuffer2 {
  private:
    std::shared_ptr<CoreGraphicsContext> m_ctx;

    std::vector<T> m_staging_buffer;

    VkDeviceSize m_size; // number of bytes
    VkBuffer m_buffer;
    VkDeviceMemory m_buffer_memory;
    void *m_buffer_mapped;

  public:
    StorageBuffer2() = default;

    StorageBuffer2(std::shared_ptr<CoreGraphicsContext> ctx, size_t capacity)
        : m_ctx(ctx), m_size(capacity * sizeof(T)) {

        m_staging_buffer.reserve(capacity);

        create_buffer(m_ctx.get(), m_size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      m_buffer, m_buffer_memory);

        vkMapMemory(m_ctx->device, m_buffer_memory, 0, m_size, 0, &m_buffer_mapped);
    }

    ~StorageBuffer2() {
        if (m_buffer == VK_NULL_HANDLE) {
            return;
        }
        vkUnmapMemory(m_ctx->device, m_buffer_memory);
        vkFreeMemory(m_ctx->device, m_buffer_memory, nullptr);
        vkDestroyBuffer(m_ctx->device, m_buffer, nullptr);
    }

    StorageBuffer2(const StorageBuffer2 &other) = delete;
    StorageBuffer2 &operator=(const StorageBuffer2 &other) = delete;

    StorageBuffer2(StorageBuffer2 &&other) noexcept
        : m_ctx(std::move(other.m_ctx)), m_size(std::move(other.m_size)),
          m_staging_buffer(std::move(other.m_staging_buffer)),
          m_buffer(std::move(other.m_buffer)),
          m_buffer_memory(std::move(other.m_buffer_memory)),
          m_buffer_mapped(std::move(other.m_buffer_mapped)) {
        other.m_buffer = VK_NULL_HANDLE;
        other.m_buffer_memory = VK_NULL_HANDLE;
        other.m_buffer_mapped = VK_NULL_HANDLE;
    }

    StorageBuffer2 &operator=(StorageBuffer2 &&other) noexcept {
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

    size_t size() const { return m_size; }
    size_t capacity() const { return m_staging_buffer.size(); }
    void clear() { m_staging_buffer.clear(); }
    void transfer() { memcpy(m_buffer_mapped, m_staging_buffer.data(), m_size); }

    void push_back(T &t) { return m_staging_buffer.push_back(t); }

    decltype(auto) push_back(T &&t) {
        return m_staging_buffer.push_back(std::forward(t));
    }

    template <typename... Args> decltype(auto) emplace_back(Args &&...args) {
        return m_staging_buffer.emplace_back(std::forward<Args>(args)...);
    }

    T &operator[](size_t index) { return m_staging_buffer[index]; }
    const T &operator[](size_t index) const { return m_staging_buffer[index]; }

    static VkDescriptorSetLayoutBinding
    create_descriptor_set_layout_binding(uint32_t binding_num) {
        VkDescriptorSetLayoutBinding layout_binding{};
        layout_binding.binding = binding_num;
        layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        layout_binding.descriptorCount = 1;
        layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        layout_binding.pImmutableSamplers = nullptr; // Optional
        return layout_binding;
    }

    void dump_data() {
        std::cout << "=== StorageBuffer2 Data Dump ===" << std::endl;
        std::cout << "Buffer size: " << m_size << " bytes" << std::endl;
        std::cout << "Staging buffer capacity: " << m_staging_buffer.capacity()
                  << std::endl;
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
            size_t element_count = m_size / sizeof(T);

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
            if (m_size % 16 != 0)
                printf("\n");
        }

        std::cout << "=== End Data Dump ===" << std::endl;
    }
};
