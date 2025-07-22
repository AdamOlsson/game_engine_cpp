#include "render_engine/PerformanceWindow.h"
#include "render_engine/RenderBody.h"
#include "render_engine/RenderEngine.h"
#include "render_engine/ui/Menu.h"
#include "render_engine/ui/UI.h"
#include <glm/glm.hpp>

PerformanceWindow::PerformanceWindow() {
    const glm::vec3 header_dimension = glm::vec3(150.0f, 20.0f, 0.0f);
    const glm::vec3 header_position = glm::vec3(-300.0f, 370.0f, 0.0f);
    const glm::vec2 header_text = glm::vec2(header_position.x, header_position.y);

    const glm::vec3 body_dimension = glm::vec3(header_dimension.x, 45.0f, 0.0f);

    const glm::vec3 header_body_position_offset =
        glm::vec3(0.0f, (header_dimension.y / 2.0f) + (body_dimension.y / 2.0f), 0.0f);

    const glm::vec3 body_position = header_position - header_body_position_offset;

    const auto font_props = ui::ElementProperties::FontProperties{
        .color = colors::WHITE, .size = 24, .weight = 1.0f, .sharpness = 1.0f};

    m_header =
        RenderBodyBuilder()
            .shape(Shape::create_rectangle_data(header_dimension.x, header_dimension.y))
            .color(colors::BLUE)
            .position(header_position)
            .build();

    m_body = RenderBodyBuilder()
                 .shape(Shape::create_rectangle_data(body_dimension.x, body_dimension.y))
                 .color(colors::rgba(0.2f, 0.2f, 0.2f, 0.2f))
                 .position(body_position)
                 .build();

    m_components = {std::cref(m_header), std::cref(m_body)};

    auto m_menu = ui::Menu();
    m_ui = ui::UI(m_menu);
    m_ui.add_text_box(
        "HEADER",
        ui::TextBox("PERFORMANCE", ui::ElementProperties{.container.center = header_text,
                                                         .container.dimension =
                                                             glm::vec2(200.0f, 200.0f),
                                                         .font = font_props}));

    const auto base_header_body_text_offset = glm::vec2(0.0f, 5.0f);
    const auto text_offset = glm::vec2(0.0f, 18.0f);
    auto left_alignment = glm::vec2(-5.0f, 0.0);
    m_ui.add_text_box(
        "FPS", ui::TextBox("FPS", ui::ElementProperties{
                                      .container.center = header_text -
                                                          base_header_body_text_offset -
                                                          text_offset + left_alignment,
                                      .container.dimension = glm::vec2(200.0f, 200.0f),
                                      .font = font_props}));

    auto right_alignment = glm::vec2(36.0f, 0.0);
    m_ui.add_text_box(
        FPS_VALUE_TAG,
        ui::TextBox(
            "60", ui::ElementProperties{.container.center = header_text -
                                                            base_header_body_text_offset -
                                                            text_offset + right_alignment,
                                        .container.dimension = glm::vec2(200.0f, 200.0f),
                                        .font = font_props}));

    left_alignment = glm::vec2(-30.0f, 0.0);
    m_ui.add_text_box(
        "FRAME_TIME",
        ui::TextBox("FRAME TIME",
                    ui::ElementProperties{
                        .container.center = header_text - base_header_body_text_offset +
                                            left_alignment - text_offset * 2.0f,
                        .container.dimension = glm::vec2(200.0f, 200.0f),
                        .font = font_props}));

    right_alignment = glm::vec2(45.0f, 0.0f);
    m_ui.add_text_box(
        FRAME_TIME_VALUE_TAG,
        ui::TextBox("00.0",
                    ui::ElementProperties{
                        .container.center = header_text - base_header_body_text_offset +
                                            right_alignment - text_offset * 2.0f,
                        .container.dimension = glm::vec2(200.0f, 200.0f),
                        .font = font_props}));

    m_last_update = Clock::now();
    m_update_interval_s = 0.5f;
    m_frame_count = 0.0f;

    m_last_fps = "60";
    m_last_frame_time = "0.16";
}

void PerformanceWindow::render(RenderEngine &render_engine) {
    TimePoint now = Clock::now();
    Duration elapsed = now - m_last_update;

    m_frame_count += 1.0f;

    if (elapsed.count() > m_update_interval_s) {
        m_last_frame_time = std::format("{:.3f}", elapsed.count() / m_frame_count);
        m_last_fps =
            std::format("{:3}", static_cast<uint32_t>(m_frame_count / elapsed.count()));
        m_frame_count = 0.0f;
        m_last_update = Clock::now();
    }

    m_ui.get_text_box(FRAME_TIME_VALUE_TAG).text = m_last_frame_time;
    m_ui.get_text_box(FPS_VALUE_TAG).text = m_last_fps;

    render_engine.render(m_components);

    auto ui_state = m_ui.get_state();
    render_engine.render_ui(ui_state);
}
