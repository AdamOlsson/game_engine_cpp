#include "DescriptorSetBuilder.h"

DescriptorSetBuilder::DescriptorSetBuilder(size_t capacity)
    : m_capacity(capacity), m_texture(nullptr), m_sampler(nullptr) {}

DescriptorSetBuilder &
DescriptorSetBuilder::add_gpu_buffer(size_t binding,
                                     std::vector<GpuBufferRef> &&buffers) {
    if (buffers.size() != m_capacity) {
        throw std::runtime_error(
            "size of storage buffer reference needs to be equal to capacity");
    }

    m_gpu_buffer_binding.insert(m_gpu_buffer_binding.end(), buffers.size(), binding);
    m_gpu_buffers.insert(m_gpu_buffers.end(), std::make_move_iterator(buffers.begin()),
                         std::make_move_iterator(buffers.end()));

    // all buffers are of the same typer every invokation
    switch (m_gpu_buffers.back().type) {
    case GpuBufferType::Storage:
        m_descriptor_set_layout_builder.add(
            BufferDescriptor<
                GpuBufferType::Storage>::create_descriptor_set_layout_binding(binding));
        break;
    case GpuBufferType::Uniform:
        m_descriptor_set_layout_builder.add(
            BufferDescriptor<
                GpuBufferType::Uniform>::create_descriptor_set_layout_binding(binding));
        break;
    }

    return *this;
}

DescriptorSetBuilder &
DescriptorSetBuilder::set_texture_and_sampler(size_t binding, Texture *texture,
                                              vulkan::Sampler *sampler) {
    m_texture_binding = binding;
    m_texture = texture;
    m_sampler = sampler;
    m_descriptor_set_layout_builder.add(
        sampler->create_descriptor_set_layout_binding(binding));
    return *this;
}

std::vector<VkDescriptorSet> DescriptorSetBuilder::allocate_descriptor_sets(
    std::shared_ptr<graphics_context::GraphicsContext> &ctx,
    DescriptorPool &descriptor_pool, const VkDescriptorSetLayout &descriptor_set_layout) {
    std::vector<VkDescriptorSetLayout> layouts(m_capacity, descriptor_set_layout);

    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = descriptor_pool;
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

VkWriteDescriptorSet DescriptorSetBuilder::create_buffer_descriptor_write(
    const VkDescriptorType type, const VkDescriptorSet &dst_descriptor_set,
    const VkDescriptorBufferInfo &buffer_info, const size_t binding_num) {
    VkWriteDescriptorSet buffer_descriptor_write{};
    buffer_descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    buffer_descriptor_write.dstSet = dst_descriptor_set;
    buffer_descriptor_write.dstBinding = binding_num;
    buffer_descriptor_write.dstArrayElement = 0;
    buffer_descriptor_write.descriptorType = type;
    buffer_descriptor_write.descriptorCount = 1;
    buffer_descriptor_write.pBufferInfo = &buffer_info;
    return buffer_descriptor_write;
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
DescriptorSetBuilder::build(std::shared_ptr<graphics_context::GraphicsContext> &ctx,
                            DescriptorPool &descriptor_pool) {

    if (m_gpu_buffers.size() % m_capacity != 0) {
        throw std::runtime_error(
            "Number of storage buffers needs to be a multiple of capacity");
    }

    DescriptorSetLayout descriptor_set_layout =
        m_descriptor_set_layout_builder.build(ctx);

    std::vector<VkDescriptorSet> descriptor_sets =
        allocate_descriptor_sets(ctx, descriptor_pool, descriptor_set_layout);

    const size_t num_buffers_per_set = m_gpu_buffers.size() / m_capacity;

    for (auto i = 0; i < m_capacity; i++) {
        std::vector<VkWriteDescriptorSet> descriptor_writes = {};

        std::vector<VkDescriptorBufferInfo> storage_buffer_infos = {};
        storage_buffer_infos.reserve(num_buffers_per_set);
        if (m_gpu_buffers.size() != 0) {
            for (auto j = 0; j < num_buffers_per_set; j++) {

                VkDescriptorBufferInfo storage_buffer_info{};
                const auto idx = j * m_capacity + i;

                storage_buffer_info.buffer = m_gpu_buffers.at(idx).buffer;
                storage_buffer_info.offset = 0;
                storage_buffer_info.range = m_gpu_buffers.at(idx).size;
                storage_buffer_infos.push_back(storage_buffer_info);
                switch (m_gpu_buffers[idx].type) {
                case GpuBufferType::Storage:
                    descriptor_writes.push_back(create_buffer_descriptor_write(
                        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, descriptor_sets[i],
                        storage_buffer_infos.back(), m_gpu_buffer_binding[idx]));
                    break;
                case GpuBufferType::Uniform:
                    descriptor_writes.push_back(create_buffer_descriptor_write(
                        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, descriptor_sets[i],
                        storage_buffer_infos.back(), m_gpu_buffer_binding[idx]));
                    break;
                }
            }
        }

        VkDescriptorImageInfo image_info{};
        if (m_texture != nullptr && m_sampler != nullptr) {
            image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            image_info.imageView = m_texture->view();
            image_info.sampler = *m_sampler;

            descriptor_writes.push_back(create_texture_and_sampler_descriptor_write(
                descriptor_sets[i], image_info));
        }

        vkUpdateDescriptorSets(ctx->logical_device, descriptor_writes.size(),
                               descriptor_writes.data(), 0, nullptr);
    }

    return DescriptorSet(ctx, descriptor_sets, std::move(descriptor_set_layout));
}
