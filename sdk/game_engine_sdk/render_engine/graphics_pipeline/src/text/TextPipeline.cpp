#include "graphics_pipeline/text/TextPipeline.h"
#include "graphics_pipeline/Polygon.h"
#include "graphics_pipeline/text/GlyphVertex.h"
#include "graphics_pipeline/winding.h"
#include "shaders/text_fragment_shader.h"
#include "shaders/text_vertex_shader.h"
#include "triangulation/mapbox/earcut.h"
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
    vulkan::CommandBufferManager *command_buffer_manager,
    font::FontLoader &&font_loader) {

    std::vector<vulkan::DrawIndexedIndirectCommand> index_draw_commands;
    size_t instance_offset_count = 0;
    std::vector<GlyphVertex> vertices;
    std::vector<uint16_t> indices;

    for (const auto glyph_index : font_loader) {
        font::GlyphOutlines outlines = font_loader.get_glyph_outline(glyph_index);

        const size_t first_index = indices.size();
        if (!outlines.line_segments.empty()) {
            const std::vector<graphics_pipeline::Polygon> glyph_polygons =
                graphics_pipeline::Polygon::construct_polygons(outlines);

            for (const graphics_pipeline::Polygon polygon : glyph_polygons) {
                const size_t first_vertex = vertices.size();

                const std::vector<unsigned int> triangle_indices =
                    mapbox::earcut(polygon.get_outlines());

                // First load write all vertices from earcut to the vertex buffer
                for (const std::vector<std::pair<float, float>> &outline :
                     polygon.get_outlines()) {
                    for (const std::pair<float, float> &vertex : outline) {
                        vertices.emplace_back(vertex.first, vertex.second, 0.0f, 0.0f,
                                              0.0f, 0.0f);
                    }
                }

                // Secondly write the indices forming the triangles from earcut into to
                // index buffer
                for (const unsigned int index : triangle_indices) {
                    indices.emplace_back(first_vertex + index);
                }

                // Thirdly write all curve segments into vertex and index buffer (which by
                // nature of bezier curves are already triangulated)
                for (const std::vector<std::array<std::pair<float, float>, 3>> &outline :
                     polygon.get_quadratic_curves()) {
                    for (const std::array<std::pair<float, float>, 3> &curve : outline) {
                        // CONTINUE:
                        // - Move winding.h to math
                        // - Make OutlineBuilder.h use the new WindingOrder to decide on
                        // how the winding order the curves are added
                        // - Make below switch-statement better
                        // - Implement a function on the TextPipeline to render all
                        // available glyphs
                        float winding_order = 0.0;
                        switch (m_font_loader->get_format()) {
                        case font::TrueType:
                            winding_order =
                                is_counter_clockwise_winding(curve) ? 0.0f : 1.0f;
                            break;
                        case font::CFF:
                            winding_order =
                                is_counter_clockwise_winding(curve) ? 1.0f : 0.0f;
                            break;
                        case font::Type1:
                        case font::Unkown:
                            break;
                        }

                        indices.emplace_back(vertices.size());
                        vertices.emplace_back(curve[0].first, curve[0].second,
                                              winding_order, 0.0f, 0.0f, 1.0f);

                        indices.emplace_back(vertices.size());
                        vertices.emplace_back(curve[1].first, curve[1].second,
                                              winding_order, 0.5f, 0.0f, 1.0f);

                        indices.emplace_back(vertices.size());
                        vertices.emplace_back(curve[2].first, curve[2].second,
                                              winding_order, 1.0f, 1.0f, 1.0f);
                    }
                }
            }
        }

        index_draw_commands.push_back(vulkan::DrawIndexedIndirectCommand{
            .indexCount = static_cast<uint32_t>(indices.size() - first_index),
            .instanceCount = 1,
            .firstIndex = static_cast<uint32_t>(first_index),
            .firstInstance = static_cast<uint32_t>(instance_offset_count),
        });

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
    m_font_loader = std::move(font_loader);
}
