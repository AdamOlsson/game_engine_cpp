#include "render_engine/GeometryPipeline.h"
#include "render_engine/Geometry.h"
#include "render_engine/Sampler.h"
#include "render_engine/Texture.h"
#include "render_engine/buffers/GpuBuffer.h"
#include "render_engine/descriptors/DescriptorPool.h"
#include "render_engine/descriptors/DescriptorSet.h"
#include "render_engine/descriptors/DescriptorSetBuilder.h"
#include "render_engine/graphics_pipeline/GraphicsPipelineBuilder.h"
#include "render_engine/resources/ResourceManager.h"
#include "shape.h"
#include "vulkan/vulkan_core.h"
#include <cstdint>
#include <cstring>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <vector>

GeometryPipeline::GeometryPipeline(
    std::shared_ptr<graphics_context::GraphicsContext> ctx,
    SwapChainManager &swap_chain_manager,
    SwapUniformBuffer<window::WindowDimension<float>> &uniform_buffers, Sampler &sampler,
    Texture &texture)
    : m_ctx(ctx),
      m_circle_instance_buffers(SwapStorageBuffer<GeometryInstanceBufferObject>(
          ctx, MAX_FRAMES_IN_FLIGHT, 1024)),
      m_triangle_instance_buffers(SwapStorageBuffer<GeometryInstanceBufferObject>(
          ctx, MAX_FRAMES_IN_FLIGHT, 1024)),
      m_rectangle_instance_buffers(SwapStorageBuffer<GeometryInstanceBufferObject>(
          ctx, MAX_FRAMES_IN_FLIGHT, 1024)),
      m_hexagon_instance_buffers(SwapStorageBuffer<GeometryInstanceBufferObject>(
          ctx, MAX_FRAMES_IN_FLIGHT, 1024)),

      m_descriptor_pool(DescriptorPool(m_ctx, m_descriptor_pool_capacity,
                                       m_num_storage_buffers, m_num_uniform_buffers,
                                       m_num_samplers)),

      m_circle_vertex_buffer(
          VertexBuffer(ctx, Geometry::circle_vertices, swap_chain_manager)),
      m_circle_index_buffer(
          IndexBuffer(ctx, Geometry::circle_indices, swap_chain_manager)),
      m_circle_descriptor_set(
          DescriptorSetBuilder(MAX_FRAMES_IN_FLIGHT)
              .add_gpu_buffer(0, m_circle_instance_buffers.get_buffer_references())
              .add_gpu_buffer(1, uniform_buffers.get_buffer_references())
              .set_texture_and_sampler(2, texture, sampler)
              .build(m_ctx, m_descriptor_pool)),

      m_triangle_vertex_buffer(
          VertexBuffer(ctx, Geometry::triangle_vertices, swap_chain_manager)),
      m_triangle_index_buffer(
          IndexBuffer(ctx, Geometry::triangle_indices, swap_chain_manager)),
      m_triangle_descriptor_set(
          DescriptorSetBuilder(MAX_FRAMES_IN_FLIGHT)
              .add_gpu_buffer(0, m_triangle_instance_buffers.get_buffer_references())
              .add_gpu_buffer(1, uniform_buffers.get_buffer_references())
              .set_texture_and_sampler(2, texture, sampler)
              .build(m_ctx, m_descriptor_pool)),

      m_rectangle_vertex_buffer(
          VertexBuffer(ctx, Geometry::rectangle_vertices, swap_chain_manager)),
      m_rectangle_index_buffer(
          IndexBuffer(ctx, Geometry::rectangle_indices, swap_chain_manager)),
      m_rectangle_descriptor_set(
          DescriptorSetBuilder(MAX_FRAMES_IN_FLIGHT)
              .add_gpu_buffer(0, m_rectangle_instance_buffers.get_buffer_references())
              .add_gpu_buffer(1, uniform_buffers.get_buffer_references())
              .set_texture_and_sampler(2, texture, sampler)
              .build(m_ctx, m_descriptor_pool)),

      m_hexagon_vertex_buffer(
          VertexBuffer(ctx, Geometry::hexagon_vertices, swap_chain_manager)),
      m_hexagon_index_buffer(
          IndexBuffer(ctx, Geometry::hexagon_indices, swap_chain_manager)),
      m_hexagon_descriptor_set(
          DescriptorSetBuilder(MAX_FRAMES_IN_FLIGHT)
              .add_gpu_buffer(0, m_hexagon_instance_buffers.get_buffer_references())
              .add_gpu_buffer(1, uniform_buffers.get_buffer_references())
              .set_texture_and_sampler(2, texture, sampler)
              .build(m_ctx, m_descriptor_pool)),
      m_graphics_pipeline(
          // clang-format off
        graphics_pipeline::GraphicsPipelineBuilder()
            .set_vertex_shader(ResourceManager::get_instance().get_resource<ShaderResource>("GeometryVertex"))
            .set_fragment_shader(ResourceManager::get_instance().get_resource<ShaderResource>("GeometryFragment"))
            .set_descriptor_set_layout(&m_hexagon_descriptor_set.get_layout()) // All descriptors have same layout
            .set_push_constants({
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .offset = 0,
                .size = sizeof(uint32_t)
                })
            .build(m_ctx, swap_chain_manager)
          // clang-format on
      ) {}

GeometryPipeline::~GeometryPipeline() {}

void GeometryPipeline::render_circles(const VkCommandBuffer &command_buffer) {
    const auto &instance_buffer = m_circle_instance_buffers.get_buffer();
    const auto num_instances = instance_buffer.num_elements();
    if (num_instances > 0) {
        m_graphics_pipeline.bind_push_constants(
            command_buffer, VK_SHADER_STAGE_VERTEX_BIT, ShapeTypeEncoding::CircleShape);
        auto descriptor_set = m_circle_descriptor_set.get();
        m_graphics_pipeline.render(command_buffer, m_circle_vertex_buffer,
                                   m_circle_index_buffer, descriptor_set, num_instances);
    }
    m_circle_instance_buffers.rotate();
}

void GeometryPipeline::render_triangles(const VkCommandBuffer &command_buffer) {
    const auto &instance_buffer = m_triangle_instance_buffers.get_buffer();
    const auto num_instances = instance_buffer.num_elements();
    if (num_instances > 0) {
        m_graphics_pipeline.bind_push_constants(
            command_buffer, VK_SHADER_STAGE_VERTEX_BIT, ShapeTypeEncoding::TriangleShape);
        auto descriptor_set = m_triangle_descriptor_set.get();
        m_graphics_pipeline.render(command_buffer, m_triangle_vertex_buffer,
                                   m_triangle_index_buffer, descriptor_set,
                                   num_instances);
    }
    m_triangle_instance_buffers.rotate();
}

void GeometryPipeline::render_rectangles(const VkCommandBuffer &command_buffer) {

    const auto &instance_buffer = m_rectangle_instance_buffers.get_buffer();
    const auto num_instances = instance_buffer.num_elements();
    if (num_instances > 0) {
        m_graphics_pipeline.bind_push_constants(command_buffer,
                                                VK_SHADER_STAGE_VERTEX_BIT,
                                                ShapeTypeEncoding::RectangleShape);
        auto descriptor_set = m_rectangle_descriptor_set.get();
        m_graphics_pipeline.render(command_buffer, m_rectangle_vertex_buffer,
                                   m_rectangle_index_buffer, descriptor_set,
                                   num_instances);
    }
    m_rectangle_instance_buffers.rotate();
}

void GeometryPipeline::render_hexagons(const VkCommandBuffer &command_buffer) {
    const auto &instance_buffer = m_hexagon_instance_buffers.get_buffer();
    const auto num_instances = instance_buffer.num_elements();
    if (num_instances > 0) {
        m_graphics_pipeline.bind_push_constants(
            command_buffer, VK_SHADER_STAGE_VERTEX_BIT, ShapeTypeEncoding::HexagonShape);
        auto descriptor_set = m_hexagon_descriptor_set.get();
        m_graphics_pipeline.render(command_buffer, m_hexagon_vertex_buffer,
                                   m_hexagon_index_buffer, descriptor_set, num_instances);
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

StorageBuffer<GeometryInstanceBufferObject> &
GeometryPipeline::get_circle_instance_buffer() {
    return m_circle_instance_buffers.get_buffer();
}

StorageBuffer<GeometryInstanceBufferObject> &
GeometryPipeline::get_triangle_instance_buffer() {
    return m_triangle_instance_buffers.get_buffer();
}

StorageBuffer<GeometryInstanceBufferObject> &
GeometryPipeline::get_rectangle_instance_buffer() {
    return m_rectangle_instance_buffers.get_buffer();
}

StorageBuffer<GeometryInstanceBufferObject> &
GeometryPipeline::get_hexagon_instance_buffer() {
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
