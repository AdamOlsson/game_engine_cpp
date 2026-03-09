
#include "Configuration.h"
#include "CaravanDefence.h"

void Configuration::setup_world_renderers(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx, CaravanDefence &game) {

    DEBUG_ASSERT(game.m_swap_chain != nullptr,
                 "Error: Swap chain needs to be initialised before world renderers.");

    DEBUG_ASSERT(
        game.m_command_buffer_manager != nullptr,
        "Error: Command buffer manager needs to be initialised before world renderers.");

    DEBUG_ASSERT(
        game.m_world_render_pass != nullptr,
        "Error: World render pass needs to be initialised before world renderers.");

    // World renderers
    vulkan::PushConstantRange push_constant_range;
    push_constant_range.offset = 0;
    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    push_constant_range.size = camera::Camera2D::matrix_size();

    graphics_pipeline::RendererOpts renderer_opts{};
    renderer_opts.push_constant_range = push_constant_range;
    renderer_opts.swap_chain.extent = game.m_swap_chain->get_extent();
    renderer_opts.swap_chain.render_pass = &game.m_world_render_pass;
    renderer_opts.quad.texture = graphics_pipeline::Texture::from_filepath(
        ctx, game.m_command_buffer_manager.get(), ASSET_FILE("sprite_sheet.png"));

    game.m_quad_renderer = std::make_unique<graphics_pipeline::quad::QuadRenderer>(
        ctx, game.m_command_buffer_manager.get(), renderer_opts);

    game.m_geom_renderer =
        std::make_unique<graphics_pipeline::geometry::GeometryRenderer>(
            ctx, game.m_command_buffer_manager.get(), renderer_opts);
}

void Configuration::setup_ui_renderers(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx, CaravanDefence &game) {

    DEBUG_ASSERT(game.m_swap_chain != nullptr,
                 "Error: Swap chain needs to be initialised before ui renderers.");

    DEBUG_ASSERT(
        game.m_command_buffer_manager != nullptr,
        "Error: Command buffer manager needs to be initialised before ui renderers.");

    DEBUG_ASSERT(game.m_ui_render_pass != nullptr,
                 "Error: UI render pass needs to be initialised before ui renderers.");

    vulkan::PushConstantRange push_constant_range;
    push_constant_range.offset = 0;
    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    push_constant_range.size = camera::Camera2D::matrix_size();

    graphics_pipeline::RendererOpts ui_renderer_opts{};
    ui_renderer_opts.push_constant_range = push_constant_range;
    ui_renderer_opts.swap_chain.extent = game.m_swap_chain->get_extent();
    ui_renderer_opts.swap_chain.render_pass = &game.m_ui_render_pass;

    game.m_ui_text_renderer =
        std::make_unique<graphics_pipeline::text::TextRenderer>(ctx, ui_renderer_opts);
    game.m_ui_text_renderer->load_font(
        game.m_command_buffer_manager.get(),
        font::FontLoader(ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf")));

    game.m_ui_geom_renderer =
        std::make_unique<graphics_pipeline::geometry::GeometryRenderer>(
            ctx, game.m_command_buffer_manager.get(), ui_renderer_opts);
}
