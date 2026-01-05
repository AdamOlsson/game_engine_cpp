#include "graphics_pipeline/text/TextPipeline.h"
#include "graphics_pipeline/text/GlyphVertex.h"
#include "shaders/text_fragment_shader.h"
#include "shaders/text_vertex_shader.h"
#include "triangulation/earcut.h"
#include "vulkan/vulkan_core.h"

graphics_pipeline::text::TextPipeline::TextPipeline(
    std::shared_ptr<vulkan::context::GraphicsContext> ctx,
    vulkan::CommandBufferManager *command_buffer_manager,
    vulkan::SwapChainManager *swap_chain_manager,
    const vulkan::DescriptorSetLayout *descriptor_set_layout,
    const vulkan::PushConstantRange *push_constant_range)
    : m_ctx(ctx), m_glyph_vertex_buffer(std::nullopt), m_glyph_index_buffer(std::nullopt),
      m_push_constant_stage(push_constant_range ? push_constant_range->stageFlags : 0),
      m_pipeline_layout(
          vulkan::PipelineLayout(ctx, descriptor_set_layout, push_constant_range)) {

    auto quad_vert = TextVertexShader::create_resource();
    auto quad_frag = TextFragmentShader::create_resource();

    VkVertexInputBindingDescription binding_description =
        GlyphVertex::get_binding_description();
    std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions =
        GlyphVertex::get_attribute_descriptions();

    m_pipeline = vulkan::Pipeline(
        ctx, m_pipeline_layout,
        vulkan::ShaderModule(ctx, quad_vert->bytes(), quad_vert->length()),
        vulkan::ShaderModule(ctx, quad_frag->bytes(), quad_frag->length()),
        *swap_chain_manager,
        vulkan::PipelineOpts{
            .vertex_input_info =
                {
                    .binding_description = binding_description,
                    .attribute_descriptions_count = attribute_descriptions.size(),
                    .attribute_descriptions = attribute_descriptions.data(),
                },
            .rasterizer = {
                .cull_mode = VK_CULL_MODE_NONE,
            }});
}

void graphics_pipeline::text::TextPipeline::load_font(
    vulkan::CommandBufferManager *command_buffer_manager, const font::OTFFont &font) {

    std::vector<vulkan::DrawIndexedIndirectCommand> index_draw_commands;
    size_t instance_offset_count = 0;
    std::vector<GlyphVertex> vertices;
    std::vector<uint16_t> indices;
    for (const auto &glyph : font.glyphs) {

        // Most OTF files contain empty glyphs, to preserve the character mapping we still
        // add an empty draw command that renders nothing
        if (glyph.polygons.size() == 0) {
            index_draw_commands.push_back(vulkan::DrawIndexedIndirectCommand{
                .indexCount = static_cast<uint32_t>(indices.size()),
                .instanceCount = 1,
                .firstIndex = static_cast<uint32_t>(indices.size()),
                .firstInstance = static_cast<uint32_t>(instance_offset_count),
            });

            instance_offset_count++;
            continue;
        }

        const size_t first_index_idx = indices.size();
        const size_t first_vertex_idx = vertices.size();

        size_t polygon_offset = 0;
        for (const font::Polygon &polygon : glyph.polygons) {

            const triangulation::Triangles<float> triangles =
                triangulation::Earcut<float>::run(polygon.exterior_outline,
                                                  polygon.interior_outlines);

            for (const auto &point : triangles.vertices) {
                vertices.emplace_back(point.first, -1.0f * point.second, 0.0f, 0.0f, 0.0f,
                                      0.0f);
            }

            for (const auto &triangle : triangles.indices) {
                indices.push_back(triangle[0] + first_vertex_idx + polygon_offset);
                indices.push_back(triangle[1] + first_vertex_idx + polygon_offset);
                indices.push_back(triangle[2] + first_vertex_idx + polygon_offset);
            }

            for (const font::ExteriorTriangle &triangle : polygon.curves) {
                const float clockwise_winding = triangle.clockwise_winding ? 1.0f : 0.0f;
                indices.push_back(vertices.size());
                vertices.emplace_back(triangle.vertices[0].first,
                                      -1.0f * triangle.vertices[0].second,
                                      clockwise_winding, triangle.uvw[0][0],
                                      triangle.uvw[0][1], triangle.uvw[0][2]);

                indices.push_back(vertices.size());
                vertices.emplace_back(triangle.vertices[1].first,
                                      -1.0f * triangle.vertices[1].second,
                                      clockwise_winding, triangle.uvw[1][0],
                                      triangle.uvw[1][1], triangle.uvw[1][2]);

                indices.push_back(vertices.size());
                vertices.emplace_back(triangle.vertices[2].first,
                                      -1.0f * triangle.vertices[2].second,
                                      clockwise_winding, triangle.uvw[2][0],
                                      triangle.uvw[2][1], triangle.uvw[2][2]);
            }

            polygon_offset = vertices.size() - first_vertex_idx;
        }

        index_draw_commands.push_back(vulkan::DrawIndexedIndirectCommand{
            .indexCount = static_cast<uint32_t>(indices.size() - first_index_idx),
            .instanceCount = 1,
            .firstIndex = static_cast<uint32_t>(first_index_idx),
            .firstInstance = static_cast<uint32_t>(instance_offset_count),
        });

        // TODO: Handle multiple instances of a glyph
        instance_offset_count++;
    }

    m_glyph_vertex_buffer =
        vulkan::buffers::VertexBuffer(m_ctx, vertices, command_buffer_manager);
    m_glyph_index_buffer =
        vulkan::buffers::IndexBuffer(m_ctx, indices, command_buffer_manager);

    m_draw_command_buffer =
        vulkan::buffers::StorageBuffer<vulkan::DrawIndexedIndirectCommand>(
            m_ctx, index_draw_commands.size(), VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT);

    for (auto &command : index_draw_commands) {
        m_draw_command_buffer->push_back(std::move(command));
    }

    m_draw_command_buffer->transfer();
    m_font = font;
}
