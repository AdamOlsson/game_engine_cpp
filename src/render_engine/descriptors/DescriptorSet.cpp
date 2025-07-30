#include "DescriptorSet.h"
#include "DescriptorPool.h"
#include "DescriptorSetLayoutBuilder.h"
#include "render_engine/Sampler.h"
#include "render_engine/Texture.h"
#include "render_engine/buffers/GpuBuffer.h"
#include "vulkan/vulkan_core.h"
#include <memory>
#include <stdexcept>

DescriptorSet::DescriptorSet(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                             std::vector<VkDescriptorSet> &descriptor_sets)
    : m_ctx(ctx), m_capacity(descriptor_sets.size()), m_next(0),
      m_descriptor_sets(std::move(descriptor_sets)) {}

const VkDescriptorSet DescriptorSet::get() {
    auto &desc = m_descriptor_sets[m_next];
    m_next = ++m_next % m_capacity;
    return desc;
}

DescriptorSetBuilder::DescriptorSetBuilder(
    const VkDescriptorSetLayout &descriptor_set_layout, DescriptorPool &descriptor_pool,
    size_t capacity)
    : m_descriptor_set_layout(descriptor_set_layout), m_descriptor_pool(&descriptor_pool),
      m_capacity(capacity),

      m_uniform_buffer_binding(0), m_instance_buffer_binding(0), m_texture_binding(0),
      m_texture(nullptr), m_sampler(nullptr) {}

DescriptorSetBuilder &
DescriptorSetBuilder::set_uniform_buffer(size_t binding,
                                         std::vector<GpuBufferRef> &&uniform_buffers) {
    m_uniform_buffer_binding = binding;
    m_uniform_buffers = std::move(uniform_buffers);
    return *this;
}

DescriptorSetBuilder &
DescriptorSetBuilder::add_storage_buffer(size_t binding,
                                         std::vector<GpuBufferRef> &&instance_buffers) {
    if (instance_buffers.size() != m_capacity) {
        throw std::runtime_error(
            "size of storage buffer reference needs to be equal to capacity");
    }

    m_instance_buffer_binding.insert(m_instance_buffer_binding.end(),
                                     instance_buffers.size(), binding);
    m_instance_buffers.insert(m_instance_buffers.end(),
                              std::make_move_iterator(instance_buffers.begin()),
                              std::make_move_iterator(instance_buffers.end()));

    return *this;
}

DescriptorSetBuilder &DescriptorSetBuilder::set_texture_and_sampler(size_t binding,
                                                                    Texture &texture,
                                                                    Sampler &sampler) {
    m_texture_binding = binding;
    m_texture = &texture;
    m_sampler = &sampler;
    return *this;
}

std::vector<VkDescriptorSet> DescriptorSetBuilder::allocate_descriptor_sets(
    std::shared_ptr<graphics_context::GraphicsContext> &ctx) {
    std::vector<VkDescriptorSetLayout> layouts(m_capacity, m_descriptor_set_layout);

    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = m_descriptor_pool->m_descriptor_pool;
    alloc_info.descriptorSetCount = static_cast<uint32_t>(m_capacity);
    alloc_info.pSetLayouts = layouts.data();

    std::vector<VkDescriptorSet> descriptor_sets;
    descriptor_sets.resize(m_capacity);
    if (vkAllocateDescriptorSets(ctx->logical_device, &alloc_info,
                                 descriptor_sets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
    return descriptor_sets;
}

VkWriteDescriptorSet DescriptorSetBuilder::create_instance_buffer_descriptor_write(
    const VkDescriptorSet &dst_descriptor_set, const VkDescriptorBufferInfo &buffer_info,
    const size_t binding_num) {
    VkWriteDescriptorSet instance_buffer_descriptor_write{};
    instance_buffer_descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    instance_buffer_descriptor_write.dstSet = dst_descriptor_set;
    instance_buffer_descriptor_write.dstBinding = binding_num;
    instance_buffer_descriptor_write.dstArrayElement = 0;
    instance_buffer_descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    instance_buffer_descriptor_write.descriptorCount = 1;
    instance_buffer_descriptor_write.pBufferInfo = &buffer_info;
    instance_buffer_descriptor_write.pImageInfo = nullptr;       // Optional
    instance_buffer_descriptor_write.pTexelBufferView = nullptr; // Optional
    return instance_buffer_descriptor_write;
}

VkWriteDescriptorSet DescriptorSetBuilder::create_uniform_buffer_descriptor_write(
    const VkDescriptorSet &dst_descriptor_set,
    const VkDescriptorBufferInfo &buffer_info) {
    VkWriteDescriptorSet uniform_buffer_descriptor_write{};
    uniform_buffer_descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    uniform_buffer_descriptor_write.dstSet = dst_descriptor_set;
    uniform_buffer_descriptor_write.dstBinding = m_uniform_buffer_binding;
    uniform_buffer_descriptor_write.dstArrayElement = 0;
    uniform_buffer_descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniform_buffer_descriptor_write.descriptorCount = 1;
    uniform_buffer_descriptor_write.pBufferInfo = &buffer_info;
    return uniform_buffer_descriptor_write;
}

VkWriteDescriptorSet DescriptorSetBuilder::create_texture_and_sampler_descriptor_write(
    const VkDescriptorSet &dst_descriptor_set, VkDescriptorImageInfo &image_info) {
    VkWriteDescriptorSet texture_descriptor_write{};
    texture_descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    texture_descriptor_write.dstSet = dst_descriptor_set;
    texture_descriptor_write.dstBinding = m_texture_binding;
    texture_descriptor_write.dstArrayElement = 0;
    texture_descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    texture_descriptor_write.descriptorCount = 1;
    texture_descriptor_write.pImageInfo = &image_info;
    return texture_descriptor_write;
}

DescriptorSet
DescriptorSetBuilder::build(std::shared_ptr<graphics_context::GraphicsContext> &ctx) {
    if (m_descriptor_set_layout == nullptr) {
        throw std::runtime_error("Descriptor set layout not set");
    }

    if (m_descriptor_pool == nullptr) {
        throw std::runtime_error("Descriptor pool not set");
    }

    if (m_instance_buffers.size() % m_capacity != 0) {
        throw std::runtime_error(
            "Number of storage buffers needs to be a multiple of capacity");
    }

    auto descriptor_set_layout_builder = DescriptorSetLayoutBuilder();

    /*if (m_sampler != nullptr && m_texture != nullptr) {*/
    /*    descriptor_set_layout_builder.add(*/
    /*        m_sampler->create_descriptor_set_layout_binding(m_texture_binding));*/
    /*}*/

    /*if (m_instance_buffers.size() != 0) {*/
    /*for (auto i = 0; i < m_instance_buffers.size(); i++) {*/
    /*descriptor_set_layout_builder.add(*/
    /*    m_instance_buffers[i].create_descriptor_set_layout_binding(*/
    /*        m_instance_buffer_binding[i]));*/
    /*}*/
    /*}*/

    std::vector<VkDescriptorSet> descriptor_sets = allocate_descriptor_sets(ctx);

    const size_t num_buffers_per_set = m_instance_buffers.size() / m_capacity;

    for (auto i = 0; i < m_capacity; i++) {
        std::vector<VkWriteDescriptorSet> descriptor_writes = {};

        std::vector<VkDescriptorBufferInfo> storage_buffer_infos = {};
        storage_buffer_infos.reserve(num_buffers_per_set);
        if (m_instance_buffers.size() != 0) {
            for (auto j = 0; j < num_buffers_per_set; j++) {

                VkDescriptorBufferInfo storage_buffer_info{};
                const auto idx = j * m_capacity + i;

                storage_buffer_info.buffer = m_instance_buffers.at(idx).buffer;
                storage_buffer_info.offset = 0;
                storage_buffer_info.range = m_instance_buffers.at(idx).size;
                storage_buffer_infos.push_back(storage_buffer_info);

                descriptor_writes.push_back(create_instance_buffer_descriptor_write(
                    descriptor_sets[i], storage_buffer_infos.back(),
                    m_instance_buffer_binding[idx]));
            }
        }

        VkDescriptorBufferInfo uniform_buffer_info{};
        if (m_uniform_buffers.size() != 0 && m_uniform_buffers.size() >= m_capacity) {
            uniform_buffer_info.buffer = m_uniform_buffers.at(i).buffer;
            uniform_buffer_info.offset = 0;
            uniform_buffer_info.range = m_uniform_buffers.at(i).size;

            descriptor_writes.push_back(create_uniform_buffer_descriptor_write(
                descriptor_sets[i], uniform_buffer_info));
        }

        VkDescriptorImageInfo image_info{};
        if (m_texture != nullptr && m_sampler != nullptr) {
            image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            image_info.imageView = m_texture->view();
            image_info.sampler = m_sampler->sampler;

            descriptor_writes.push_back(create_texture_and_sampler_descriptor_write(
                descriptor_sets[i], image_info));
        }

        vkUpdateDescriptorSets(ctx->logical_device, descriptor_writes.size(),
                               descriptor_writes.data(), 0, nullptr);
    }

    return DescriptorSet(ctx, descriptor_sets);
}
