#include "render_engine/RenderEngine.h"
#include "render_engine/RenderBody.h"
#include "render_engine/SwapChainManager.h"
#include "render_engine/buffers/GpuBuffer.h"
#include "render_engine/fonts/Font.h"
#include "render_engine/ui/TextBox.h"
#include "render_engine/window/Window.h"
#include <memory>

RenderEngine::RenderEngine(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                           CommandBufferManager *command_buffer_manager,
                           SwapChainManager *swap_chain_manager, const UseFont use_font)
    : m_window_dimension_buffers(SwapUniformBuffer<window::WindowDimension<float>>(
          ctx, graphics_pipeline::MAX_FRAMES_IN_FLIGHT, 1)),
      m_sampler(Sampler(ctx)) {

    m_window_dimension_buffers.write(ctx->window->dimensions<float>());

    m_texture =
        Texture::unique_from_image_resource_name(ctx, command_buffer_manager, "DogImage");

    // TODO: I do not want the sampler, texture or uniform to be part of the constructor
    // interface, instead they should be optional to add through a builder class like
    // below
    /*m_geometry_pipeline = GeometryPipelineBuilder()*/
    /*                          .set_texture_and_sampler(< texture and sampler >)*/
    /*                          .build(ctx, command_buffer_manager, swap_chain_manager,*/
    /*                                 m_window_dimension_buffers);*/

    // TODO: Handle recreation of swap chain, like for resizing windows and moving window
    // between displays
    m_geometry_pipeline = std::make_unique<graphics_pipeline::GeometryPipeline>(
        ctx, command_buffer_manager, *swap_chain_manager, m_window_dimension_buffers,
        m_sampler, *m_texture);

    auto font =
        std::make_unique<Font>(ctx, command_buffer_manager, "DefaultFont", &m_sampler);
    m_text_pipeline = std::make_unique<graphics_pipeline::TextPipeline>(
        ctx, command_buffer_manager, *swap_chain_manager, m_window_dimension_buffers,
        std::move(font));

    // TODO: UIPipeline is dependent on TextPipeline. Should it?... Does mean we refactor
    // the geometry and UI pipeline shaders to be the same?
    m_ui_pipeline = std::make_unique<graphics_pipeline::UIPipeline>(
        ctx, command_buffer_manager, *swap_chain_manager, m_window_dimension_buffers);
}

RenderEngine::~RenderEngine() {}

void RenderEngine::render(
    CommandBuffer &command_buffer,
    const std::vector<std::reference_wrapper<const RenderBody>> &bodies) {
    auto &circle_instance_buffer = m_geometry_pipeline->get_circle_instance_buffer();
    auto &triangle_instance_buffer = m_geometry_pipeline->get_triangle_instance_buffer();
    auto &rectangle_instance_buffer =
        m_geometry_pipeline->get_rectangle_instance_buffer();
    auto &hexagon_instance_buffer = m_geometry_pipeline->get_hexagon_instance_buffer();

    circle_instance_buffer.clear();
    triangle_instance_buffer.clear();
    rectangle_instance_buffer.clear();
    hexagon_instance_buffer.clear();

    for (auto b : bodies) {
        auto deref_b = b.get();
        switch (deref_b.shape.encode_shape_type()) {
        case ShapeTypeEncoding::CircleShape:
            circle_instance_buffer.emplace_back(deref_b.position, deref_b.color,
                                                deref_b.rotation, deref_b.shape,
                                                deref_b.uvwt);
            break;
        case ShapeTypeEncoding::TriangleShape:
            triangle_instance_buffer.emplace_back(deref_b.position, deref_b.color,
                                                  deref_b.rotation, deref_b.shape,
                                                  deref_b.uvwt);
            break;
        case ShapeTypeEncoding::RectangleShape:
            rectangle_instance_buffer.emplace_back(deref_b.position, deref_b.color,
                                                   deref_b.rotation, deref_b.shape,
                                                   deref_b.uvwt);
            break;
        case ShapeTypeEncoding::HexagonShape:
            hexagon_instance_buffer.emplace_back(deref_b.position, deref_b.color,
                                                 deref_b.rotation, deref_b.shape,
                                                 deref_b.uvwt);
            break;
        };
    }

    circle_instance_buffer.transfer();
    triangle_instance_buffer.transfer();
    rectangle_instance_buffer.transfer();
    hexagon_instance_buffer.transfer();

    m_geometry_pipeline->render_circles(command_buffer);
    m_geometry_pipeline->render_triangles(command_buffer);
    m_geometry_pipeline->render_rectangles(command_buffer);
    m_geometry_pipeline->render_hexagons(command_buffer);
}

// TODO: This function is not compatible with render_ui. It is not possible to run them in
// the same loop iteration as they overwrite their buffers
void RenderEngine::render_text(const ui::TextBox &text_box) {
    auto &character_instance_buffer = m_text_pipeline->get_character_buffer();
    auto &text_segment_buffer = m_text_pipeline->get_text_segment_buffer();
    character_instance_buffer.clear();
    text_segment_buffer.clear();

    m_text_pipeline->text_kerning(text_box.text, text_box.properties);

    character_instance_buffer.transfer();
    text_segment_buffer.transfer();

    m_text_pipeline->render_text(m_current_render_pass.command_buffer);
}

// TODO: Should we instead simply pass the UI class instead of its state?
void RenderEngine::render_ui(CommandBuffer &command_buffer, const ui::State &state) {
    auto &character_instance_buffer = m_text_pipeline->get_character_buffer();
    auto &text_segment_buffer = m_text_pipeline->get_text_segment_buffer();
    character_instance_buffer.clear();
    text_segment_buffer.clear();

    for (const auto button : state.buttons) {

        m_ui_pipeline->render(command_buffer, button->properties.container);

        m_text_pipeline->text_kerning(button->text, button->properties);
    }

    for (const auto text_box : state.text_boxes) {

        m_text_pipeline->text_kerning(text_box->text, text_box->properties);
    }

    character_instance_buffer.transfer();
    text_segment_buffer.transfer();

    m_text_pipeline->render_text(command_buffer);
}
