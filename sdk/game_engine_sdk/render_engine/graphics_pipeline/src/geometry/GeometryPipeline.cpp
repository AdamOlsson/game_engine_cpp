#include "graphics_pipeline/geometry/GeometryPipeline.h"
#include "shaders/geometry_fragment_shader.h"
#include "shaders/geometry_vertex_shader.h"
#include "vulkan/Vertex.h"

namespace {
const std::vector<uint16_t> m_quad_indices = {0, 1, 2, 0, 2, 3};
const std::vector<Vertex> m_quad_vertices = {
    Vertex(-0.5f, -0.5f, 0.0f),
    Vertex(-0.5f, 0.5f, 0.0f),
    Vertex(0.5f, 0.5f, 0.0f),
    Vertex(0.5f, -0.5f, 0.0f),
};
} // namespace

graphics_pipeline::geometry::GeometryPipeline::GeometryPipeline(
    std::shared_ptr<vulkan::context::GraphicsContext> ctx,
    vulkan::CommandBufferManager *command_buffer_manager,
    vulkan::SwapChainManager *swap_chain_manager,
    const vulkan::DescriptorSetLayout *descriptor_set_layout,
    const vulkan::PushConstantRange *push_constant_range)
    : m_ctx(ctx), m_quad_vertex_buffer(vulkan::buffers::VertexBuffer(
                      m_ctx, m_quad_vertices, command_buffer_manager)),
      m_quad_index_buffer(
          vulkan::buffers::IndexBuffer(m_ctx, m_quad_indices, command_buffer_manager)),
      m_push_constant_stage(push_constant_range ? push_constant_range->stageFlags : 0),
      m_pipeline_layout(
          vulkan::PipelineLayout(ctx, descriptor_set_layout, push_constant_range)) {

    auto vert = GeometryVertexShader::create_resource();
    auto frag = GeometryFragmentShader::create_resource();

    m_pipeline = vulkan::Pipeline(
        ctx, m_pipeline_layout, vulkan::ShaderModule(ctx, vert->bytes(), vert->length()),
        vulkan::ShaderModule(ctx, frag->bytes(), frag->length()), *swap_chain_manager,
        vulkan::PipelineOpts{});
}
