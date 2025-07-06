#include "render_engine/GeometryPipeline.h"
#include "CoreGraphicsContext.h"
#include "render_engine/DescriptorPool.h"
#include "render_engine/DescriptorSet.h"
#include "render_engine/RenderableGeometry.h"
#include "render_engine/Sampler.h"
#include "render_engine/Texture.h"
#include "render_engine/buffers/StorageBuffer.h"
#include "render_engine/buffers/UniformBuffer.h"
#include "render_engine/resources/ResourceManager.h"
#include "vulkan/vulkan_core.h"
#include <cstdint>
#include <cstring>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <vector>

GeometryPipeline::GeometryPipeline(Window &window,
                                   std::shared_ptr<CoreGraphicsContext> ctx,
                                   SwapChainManager &swap_chain_manager,
                                   std::vector<UniformBuffer> &uniform_buffers,
                                   VkDescriptorSetLayout &descriptor_set_layout,
                                   Sampler &sampler, Texture &texture)
    : m_ctx(ctx), m_circle_instance_buffers(SwapStorageBuffer<StorageBufferObject>(
                      ctx, MAX_FRAMES_IN_FLIGHT, 1024)),
      m_triangle_instance_buffers(
          SwapStorageBuffer<StorageBufferObject>(ctx, MAX_FRAMES_IN_FLIGHT, 1024)),
      m_rectangle_instance_buffers(
          SwapStorageBuffer<StorageBufferObject>(ctx, MAX_FRAMES_IN_FLIGHT, 1024)),
      m_hexagon_instance_buffers(
          SwapStorageBuffer<StorageBufferObject>(ctx, MAX_FRAMES_IN_FLIGHT, 1024)),
      m_pipeline(create_pipeline(descriptor_set_layout, swap_chain_manager)),
      m_descriptor_pool(DescriptorPool(m_ctx, MAX_FRAMES_IN_FLIGHT * 6)) {

    circle_geometry = std::make_unique<Geometry::Circle>(
        ctx, swap_chain_manager,
        DescriptorSetBuilder(descriptor_set_layout, m_descriptor_pool,
                             MAX_FRAMES_IN_FLIGHT)
            .add_storage_buffers(0, m_circle_instance_buffers.get_buffer_references())
            .set_uniform_buffers(1, uniform_buffers)
            .set_texture_and_sampler(2, texture, sampler)
            .build(m_ctx));

    triangle_geometry = std::make_unique<Geometry::Triangle>(
        ctx, swap_chain_manager,
        DescriptorSetBuilder(descriptor_set_layout, m_descriptor_pool,
                             MAX_FRAMES_IN_FLIGHT)
            .add_storage_buffers(0, m_triangle_instance_buffers.get_buffer_references())
            .set_uniform_buffers(1, uniform_buffers)
            .set_texture_and_sampler(2, texture, sampler)
            .build(m_ctx));

    rectangle_geometry = std::make_unique<Geometry::Rectangle>(
        ctx, swap_chain_manager,
        DescriptorSetBuilder(descriptor_set_layout, m_descriptor_pool,
                             MAX_FRAMES_IN_FLIGHT)
            .add_storage_buffers(0, m_rectangle_instance_buffers.get_buffer_references())
            .set_uniform_buffers(1, uniform_buffers)
            .set_texture_and_sampler(2, texture, sampler)
            .build(m_ctx));

    hexagon_geometry = std::make_unique<Geometry::Hexagon>(
        ctx, swap_chain_manager,
        DescriptorSetBuilder(descriptor_set_layout, m_descriptor_pool,
                             MAX_FRAMES_IN_FLIGHT)
            .add_storage_buffers(0, m_hexagon_instance_buffers.get_buffer_references())
            .set_uniform_buffers(1, uniform_buffers)
            .set_texture_and_sampler(2, texture, sampler)
            .build(m_ctx));
}

GeometryPipeline::~GeometryPipeline() {}

Pipeline GeometryPipeline::create_pipeline(VkDescriptorSetLayout &descriptor_set_layout,
                                           SwapChainManager &swap_chain_manager) {
    auto &resoure_manager = ResourceManager::get_instance();
    auto vert_shader_code =
        resoure_manager.get_resource<ShaderResource>("GeometryVertex");
    auto frag_shader_code =
        resoure_manager.get_resource<ShaderResource>("GeometryFragment");

    VkShaderModule vert_shader_module = createShaderModule(
        m_ctx->device, vert_shader_code->bytes(), vert_shader_code->length());

    VkShaderModule frag_shader_module = createShaderModule(
        m_ctx->device, frag_shader_code->bytes(), frag_shader_code->length());

    VkPushConstantRange push_constant_range{};
    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    push_constant_range.offset = 0;
    push_constant_range.size = sizeof(uint32_t);

    Pipeline pipeline =
        Pipeline(m_ctx, descriptor_set_layout, {push_constant_range}, vert_shader_module,
                 frag_shader_module, swap_chain_manager);

    vkDestroyShaderModule(m_ctx->device, vert_shader_module, nullptr);
    vkDestroyShaderModule(m_ctx->device, frag_shader_module, nullptr);
    return pipeline;
}

StorageBuffer<StorageBufferObject> &GeometryPipeline::get_circle_instance_buffer() {
    return m_circle_instance_buffers.get_buffer();
}

StorageBuffer<StorageBufferObject> &GeometryPipeline::get_triangle_instance_buffer() {
    return m_triangle_instance_buffers.get_buffer();
}

StorageBuffer<StorageBufferObject> &GeometryPipeline::get_rectangle_instance_buffer() {
    return m_rectangle_instance_buffers.get_buffer();
}

StorageBuffer<StorageBufferObject> &GeometryPipeline::get_hexagon_instance_buffer() {
    return m_hexagon_instance_buffers.get_buffer();
}

void GeometryPipeline::render_circles(const VkCommandBuffer &command_buffer) {

    const auto &instance_buffer = m_circle_instance_buffers.get_buffer();
    const auto num_instances = instance_buffer.num_elements();
    if (num_instances > 0) {
        circle_geometry->record_draw_command(command_buffer, m_pipeline.m_pipeline,
                                             m_pipeline.m_pipeline_layout, num_instances);
    }
    m_circle_instance_buffers.rotate();
}

void GeometryPipeline::render_triangles(const VkCommandBuffer &command_buffer) {

    const auto &instance_buffer = m_triangle_instance_buffers.get_buffer();
    const auto num_instances = instance_buffer.num_elements();
    if (num_instances > 0) {
        triangle_geometry->record_draw_command(command_buffer, m_pipeline.m_pipeline,
                                               m_pipeline.m_pipeline_layout,
                                               num_instances);
    }
    m_triangle_instance_buffers.rotate();
}

void GeometryPipeline::render_rectangles(const VkCommandBuffer &command_buffer) {

    const auto &instance_buffer = m_rectangle_instance_buffers.get_buffer();
    const auto num_instances = instance_buffer.num_elements();
    if (num_instances > 0) {
        rectangle_geometry->record_draw_command(command_buffer, m_pipeline.m_pipeline,
                                                m_pipeline.m_pipeline_layout,
                                                num_instances);
    }
    m_rectangle_instance_buffers.rotate();
}

void GeometryPipeline::render_hexagons(const VkCommandBuffer &command_buffer) {

    const auto &instance_buffer = m_hexagon_instance_buffers.get_buffer();
    const auto num_instances = instance_buffer.num_elements();
    if (num_instances > 0) {
        hexagon_geometry->record_draw_command(command_buffer, m_pipeline.m_pipeline,
                                              m_pipeline.m_pipeline_layout,
                                              num_instances);
    }
    m_hexagon_instance_buffers.rotate();
}

VkDescriptorPool createDescriptorPool(VkDevice &device, const int capacity) {
    std::array<VkDescriptorPoolSize, 3> poolSizes{};

    poolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(capacity);

    poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(capacity);

    // TODO: Do we really need this many samplers?
    poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[2].descriptorCount = static_cast<uint32_t>(capacity);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(capacity);

    VkDescriptorPool descriptorPool;
    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
    return descriptorPool;
}

VkShaderModule createShaderModule(const VkDevice &device, const uint8_t *data,
                                  const size_t len) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = len;
    createInfo.pCode = reinterpret_cast<const uint32_t *>(data);

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

std::vector<char> readFile(const std::string filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}
