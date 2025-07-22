#include "render_engine/GeometryPipeline.h"
#include "CoreGraphicsContext.h"
#include "render_engine/DescriptorPool.h"
#include "render_engine/DescriptorSet.h"
#include "render_engine/DescriptorSetLayoutBuilder.h"
#include "render_engine/Geometry.h"
#include "render_engine/Sampler.h"
#include "render_engine/ShaderModule.h"
#include "render_engine/Texture.h"
#include "render_engine/buffers/StorageBuffer.h"
#include "render_engine/buffers/UniformBuffer.h"
#include "render_engine/resources/ResourceManager.h"
#include "shape.h"
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
                                   Sampler &sampler, Texture &texture)
    : m_ctx(ctx), m_circle_instance_buffers(SwapGpuBuffer<GeometryInstanceBufferObject>(
                      ctx, MAX_FRAMES_IN_FLIGHT, 1024)),
      m_triangle_instance_buffers(
          SwapGpuBuffer<GeometryInstanceBufferObject>(ctx, MAX_FRAMES_IN_FLIGHT, 1024)),
      m_rectangle_instance_buffers(
          SwapGpuBuffer<GeometryInstanceBufferObject>(ctx, MAX_FRAMES_IN_FLIGHT, 1024)),
      m_hexagon_instance_buffers(
          SwapGpuBuffer<GeometryInstanceBufferObject>(ctx, MAX_FRAMES_IN_FLIGHT, 1024)),

      m_descriptor_set_layout(create_descriptor_set_layout()),
      m_pipeline(create_pipeline(m_descriptor_set_layout, swap_chain_manager)),
      m_descriptor_pool(DescriptorPool(m_ctx, m_descriptor_pool_capacity,
                                       m_num_storage_buffers, m_num_uniform_buffers,
                                       m_num_samplers)),

      m_circle_vertex_buffer(
          VertexBuffer(ctx, Geometry::circle_vertices, swap_chain_manager)),
      m_circle_index_buffer(
          IndexBuffer(ctx, Geometry::circle_indices, swap_chain_manager)),
      m_circle_descriptor_set(
          DescriptorSetBuilder(m_descriptor_set_layout, m_descriptor_pool,
                               MAX_FRAMES_IN_FLIGHT)
              .add_storage_buffers(0, m_circle_instance_buffers.get_buffer_references())
              .set_uniform_buffers(1, uniform_buffers)
              .set_texture_and_sampler(2, texture, sampler)
              .build(m_ctx)),

      m_triangle_vertex_buffer(
          VertexBuffer(ctx, Geometry::triangle_vertices, swap_chain_manager)),
      m_triangle_index_buffer(
          IndexBuffer(ctx, Geometry::triangle_indices, swap_chain_manager)),
      m_triangle_descriptor_set(
          DescriptorSetBuilder(m_descriptor_set_layout, m_descriptor_pool,
                               MAX_FRAMES_IN_FLIGHT)
              .add_storage_buffers(0, m_triangle_instance_buffers.get_buffer_references())
              .set_uniform_buffers(1, uniform_buffers)
              .set_texture_and_sampler(2, texture, sampler)
              .build(m_ctx)),

      m_rectangle_vertex_buffer(
          VertexBuffer(ctx, Geometry::rectangle_vertices, swap_chain_manager)),
      m_rectangle_index_buffer(
          IndexBuffer(ctx, Geometry::rectangle_indices, swap_chain_manager)),
      m_rectangle_descriptor_set(
          DescriptorSetBuilder(m_descriptor_set_layout, m_descriptor_pool,
                               MAX_FRAMES_IN_FLIGHT)
              .add_storage_buffers(0,
                                   m_rectangle_instance_buffers.get_buffer_references())
              .set_uniform_buffers(1, uniform_buffers)
              .set_texture_and_sampler(2, texture, sampler)
              .build(m_ctx)),

      m_hexagon_vertex_buffer(
          VertexBuffer(ctx, Geometry::hexagon_vertices, swap_chain_manager)),
      m_hexagon_index_buffer(
          IndexBuffer(ctx, Geometry::hexagon_indices, swap_chain_manager)),
      m_hexagon_descriptor_set(
          DescriptorSetBuilder(m_descriptor_set_layout, m_descriptor_pool,
                               MAX_FRAMES_IN_FLIGHT)
              .add_storage_buffers(0, m_hexagon_instance_buffers.get_buffer_references())
              .set_uniform_buffers(1, uniform_buffers)
              .set_texture_and_sampler(2, texture, sampler)
              .build(m_ctx))

{}

GeometryPipeline::~GeometryPipeline() {}

VkDescriptorSetLayout GeometryPipeline::create_descriptor_set_layout() {
    return DescriptorSetLayoutBuilder()
        .add(
            GpuBuffer<GeometryInstanceBufferObject>::create_descriptor_set_layout_binding(
                0))
        .add(UniformBuffer::create_descriptor_set_layout_binding(1))
        .add(Sampler::create_descriptor_set_layout_binding(2))
        .build(m_ctx.get());
}

void GeometryPipeline::record_draw_command(const VkCommandBuffer &command_buffer,
                                           DescriptorSet &descriptor_set,
                                           const ShapeTypeEncoding shape_type_encoding,
                                           const VertexBuffer &vertex_buffer,
                                           const IndexBuffer &index_buffer,
                                           const size_t num_instances) {
    const VkDeviceSize vertex_buffers_offset = 0;
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      m_pipeline.m_pipeline);

    auto desc = descriptor_set.get();
    vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            m_pipeline.m_pipeline_layout, 0, 1, &desc, 0, nullptr);
    vkCmdPushConstants(command_buffer, m_pipeline.m_pipeline_layout,
                       VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(uint32_t),
                       &shape_type_encoding);

    vkCmdBindVertexBuffers(command_buffer, 0, 1, &vertex_buffer.buffer,
                           &vertex_buffers_offset);
    vkCmdBindIndexBuffer(command_buffer, index_buffer.buffer, 0, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(command_buffer, index_buffer.num_indices, num_instances, 0, 0, 0);
}

void GeometryPipeline::render_circles(const VkCommandBuffer &command_buffer) {
    const auto &instance_buffer = m_circle_instance_buffers.get_buffer();
    const auto num_instances = instance_buffer.num_elements();
    if (num_instances > 0) {
        record_draw_command(command_buffer, m_circle_descriptor_set,
                            ShapeTypeEncoding::CircleShape, m_circle_vertex_buffer,
                            m_circle_index_buffer, num_instances);
    }
    m_circle_instance_buffers.rotate();
}

void GeometryPipeline::render_triangles(const VkCommandBuffer &command_buffer) {
    const auto &instance_buffer = m_triangle_instance_buffers.get_buffer();
    const auto num_instances = instance_buffer.num_elements();
    if (num_instances > 0) {
        record_draw_command(command_buffer, m_triangle_descriptor_set,
                            ShapeTypeEncoding::TriangleShape, m_triangle_vertex_buffer,
                            m_triangle_index_buffer, num_instances);
    }
    m_triangle_instance_buffers.rotate();
}

void GeometryPipeline::render_rectangles(const VkCommandBuffer &command_buffer) {

    const auto &instance_buffer = m_rectangle_instance_buffers.get_buffer();
    const auto num_instances = instance_buffer.num_elements();
    if (num_instances > 0) {
        record_draw_command(command_buffer, m_rectangle_descriptor_set,
                            ShapeTypeEncoding::RectangleShape, m_rectangle_vertex_buffer,
                            m_rectangle_index_buffer, num_instances);
    }
    m_rectangle_instance_buffers.rotate();
}

void GeometryPipeline::render_hexagons(const VkCommandBuffer &command_buffer) {

    const auto &instance_buffer = m_hexagon_instance_buffers.get_buffer();
    const auto num_instances = instance_buffer.num_elements();
    if (num_instances > 0) {
        record_draw_command(command_buffer, m_hexagon_descriptor_set,
                            ShapeTypeEncoding::HexagonShape, m_hexagon_vertex_buffer,
                            m_hexagon_index_buffer, num_instances);
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

Pipeline GeometryPipeline::create_pipeline(VkDescriptorSetLayout &descriptor_set_layout,
                                           SwapChainManager &swap_chain_manager) {
    auto &resoure_manager = ResourceManager::get_instance();
    auto vert_shader_code =
        resoure_manager.get_resource<ShaderResource>("GeometryVertex");
    auto frag_shader_code =
        resoure_manager.get_resource<ShaderResource>("GeometryFragment");

    ShaderModule vertex_shader = ShaderModule(m_ctx, *vert_shader_code);
    ShaderModule fragment_shader = ShaderModule(m_ctx, *frag_shader_code);

    VkPushConstantRange push_constant_range{};
    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    push_constant_range.offset = 0;
    push_constant_range.size = sizeof(uint32_t);

    Pipeline pipeline = Pipeline(m_ctx, descriptor_set_layout, {push_constant_range},
                                 vertex_shader.shader_module,
                                 fragment_shader.shader_module, swap_chain_manager);

    return pipeline;
}

GpuBuffer<GeometryInstanceBufferObject> &GeometryPipeline::get_circle_instance_buffer() {
    return m_circle_instance_buffers.get_buffer();
}

GpuBuffer<GeometryInstanceBufferObject> &
GeometryPipeline::get_triangle_instance_buffer() {
    return m_triangle_instance_buffers.get_buffer();
}

GpuBuffer<GeometryInstanceBufferObject> &
GeometryPipeline::get_rectangle_instance_buffer() {
    return m_rectangle_instance_buffers.get_buffer();
}

GpuBuffer<GeometryInstanceBufferObject> &GeometryPipeline::get_hexagon_instance_buffer() {
    return m_hexagon_instance_buffers.get_buffer();
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
