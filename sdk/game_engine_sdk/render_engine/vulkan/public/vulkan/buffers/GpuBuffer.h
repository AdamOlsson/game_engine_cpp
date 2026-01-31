#pragma once

#include "util/assert.h"
#include "vulkan/buffers/common.h"
#include "vulkan/context/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <memory>
namespace vulkan::buffers {

enum class GpuBufferType { Uniform, Storage };

struct GpuBufferRef {
    VkDeviceSize size;
    VkBuffer buffer;
    GpuBufferType type;
};

template <GpuBufferType BufferType> class BufferDescriptor;

template <typename T, GpuBufferType BufferType> class GpuBuffer {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;

    std::vector<T> m_staging_buffer;
    std::vector<size_t> m_delta_ids;

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
        }

        if (usage.has_value()) {
            usage_ = static_cast<VkBufferUsageFlagBits>(usage_ | usage.value());
        }

        m_staging_buffer.reserve(capacity);
        m_delta_ids.reserve(capacity);

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
          m_staging_buffer(std::move(other.m_staging_buffer)),
          m_buffers(std::move(other.m_buffers)), m_capacity(other.m_capacity),
          m_swap_state(other.m_swap_state), m_refs(std::move(m_refs)) {
        other.m_buffers.clear();
        other.m_capacity = 0;
    }

    GpuBuffer &operator=(GpuBuffer &&other) noexcept {
        if (this != &other) {
            m_ctx = std::move(other.m_ctx);
            m_size = std::move(other.m_size);
            m_staging_buffer = std::move(other.m_staging_buffer);
            m_buffers = std::move(other.m_buffers);
            m_swap_state = other.m_swap_state;
            m_refs = std::move(other.m_refs);
            m_capacity = other.m_capacity;

            other.m_capacity = 0;
        }
        return *this;
    }

    size_t size() const { return m_size; }
    std::vector<GpuBufferRef> get_reference() { return m_refs; }
    size_t num_elements() const { return m_staging_buffer.size(); }

    size_t size_of_T() { return sizeof(T); }

    void clear() {
        // Note: Not sure how this would work with partial updates. Right now, if you
        // clear the staging buffer you need to call transfer() to sync tracking
        // of changes to the staging buffer.
        m_staging_buffer.clear();
    }

    void rotate() { m_swap_state.rotate(); }

    void sync() {
        // Transfer the current state of the staging buffer to the next device buffer not
        // currently displayed
        const size_t next_buffer = m_swap_state.next();
        memcpy(m_buffers[next_buffer].buffer_mapped, m_staging_buffer.data(), m_size);
        m_delta_ids.clear();
    }

    void sync_all() {
        // Transfer the current state of the staging buffer to the all device buffers in
        // the swap not currently displayed
        for (auto i = 0; i < m_swap_state.swap_size; i++) {
            memcpy(m_buffers[i].buffer_mapped, m_staging_buffer.data(), m_size);
        }
        m_delta_ids.clear();
    }

    void sync_delta() {
        // Transfer the delta state of the staging buffer to the next device buffer not
        // currently displayed
        const size_t next_buffer = m_swap_state.next();
        T *device_buffer = static_cast<T *>(m_buffers[next_buffer].buffer_mapped);
        for (auto id : m_delta_ids) {
            device_buffer[id] = m_staging_buffer[id];
        }
        m_delta_ids.clear();
    }

    void push_back(T &t) {
        DEBUG_ASSERT(m_staging_buffer.size() < m_capacity,
                     "Exceeding the GPU buffers size!");
        m_delta_ids.push_back(m_staging_buffer.size() + 1);
        return m_staging_buffer.push_back(t);
    }

    void push_back(const T &t) {
        DEBUG_ASSERT(m_staging_buffer.size() < m_capacity,
                     "Exceeding the GPU buffers size!");
        m_delta_ids.push_back(m_staging_buffer.size() + 1);
        return m_staging_buffer.push_back(t);
    }

    decltype(auto) push_back(T &&t) {
        DEBUG_ASSERT(m_staging_buffer.size() < m_capacity,
                     "Exceeding the GPU buffers size!");
        m_delta_ids.push_back(m_staging_buffer.size() + 1);
        return m_staging_buffer.push_back(std::forward<T>(t));
    }

    template <typename... Args> decltype(auto) emplace_back(Args &&...args) {
        DEBUG_ASSERT(m_staging_buffer.size() < m_capacity,
                     "Exceeding the GPU buffers size!");
        m_delta_ids.push_back(m_staging_buffer.size() + 1);
        return m_staging_buffer.emplace_back(std::forward<Args>(args)...);
    }

    T &operator[](size_t index) {
        // Note: Its not necesarrily true that if a user indexes into the staging buffer,
        // that they will update it. However, there is no other way of tracking if they do
        // so we have to be defensive and asssume that changes are made. The type of
        // indexing I refer to is:
        //
        // auto& buffer_memeber = gpu_buffer[index];
        //
        // In the const case, its not possible to update the reference so no need assume
        // changes.
        m_delta_ids.push_back(index);
        return m_staging_buffer[index];
    }

    T &at(size_t index) {
        // Note: Its not necesarrily true that if a user indexes into the staging buffer,
        // that they will update it. However, there is no other way of tracking if they do
        // so we have to be defensive and asssume that changes are made. The type of
        // indexing I refer to is:
        //
        // auto& buffer_memeber = gpu_buffer[index];
        //
        // In the const case, its not possible to update the reference so no need assume
        // changes.
        m_delta_ids.push_back(index);
        return m_staging_buffer[index];
    }

    const T &operator[](size_t index) const { return m_staging_buffer[index]; }
    const T &at(size_t index) const { return m_staging_buffer[index]; }

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
        /*if (m_buffer_mapped != nullptr) {*/
        /*    std::cout << "\nGPU buffer contents:" << std::endl;*/
        /*    auto *mapped_data = static_cast<T *>(m_buffer_mapped);*/
        /*    size_t element_count = m_staging_buffer.size();*/
        /**/
        /*    for (size_t i = 0; i < element_count; ++i) {*/
        /*        std::cout << "[" << i << "]: " << mapped_data[i] << std::endl;*/
        /*    }*/
        /**/
        /*    // Also show raw byte data for debugging*/
        /*    std::cout << "\nRaw byte data (first "*/
        /*              << std::min(m_size, static_cast<VkDeviceSize>(64))*/
        /*              << " bytes):" << std::endl;*/
        /*    unsigned char *byteData = static_cast<unsigned char *>(m_buffer_mapped);*/
        /*    for (size_t i = 0; i < std::min(m_size, static_cast<VkDeviceSize>(64)); i++)
         * {*/
        /*        printf("%02x ", byteData[i]);*/
        /*        if ((i + 1) % 16 == 0)*/
        /*            printf("\n");*/
        /*    }*/
        /*    if (m_size % 16 != 0) {*/
        /*        printf("\n");*/
        /*    }*/
        /*}*/

        std::cout << "=== End Data Dump ===" << std::endl;
    }
};

template <typename T> using StorageBuffer = GpuBuffer<T, GpuBufferType::Storage>;
template <typename T> using UniformBuffer = GpuBuffer<T, GpuBufferType::Uniform>;

template <GpuBufferType BufferType> class BufferDescriptor2 {
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

} // namespace vulkan::buffers
