#include "render_engine/PerformanceWindow.h"
#include "render_engine/RenderBody.h"
#include "render_engine/RenderEngine.h"
#include "render_engine/ui/Menu.h"
#include "render_engine/ui/UI.h"
#include <functional>
#include <glm/glm.hpp>

namespace {
constexpr glm::vec3 HEADER_DIMENSION = glm::vec3(150.0f, 20.0f, 0.0f);
constexpr glm::vec3 HEADER_POSITION = glm::vec3(-300.0f, 300.0f, 0.0f);
constexpr glm::vec2 HEADER_TEXT = glm::vec2(HEADER_POSITION.x, -HEADER_POSITION.y);

constexpr glm::vec3 BODY_DIMENSION = glm::vec3(150.0f, 100.0f, 0.0f);

constexpr glm::vec3 HEADER_BODY_POSITION_OFFSET =
    glm::vec3(0.0f, (HEADER_DIMENSION.y / 2.0f) + (BODY_DIMENSION.y / 2.0f), 0.0f);

constexpr glm::vec3 BODY_POSITION = HEADER_POSITION - HEADER_BODY_POSITION_OFFSET;
} // namespace

void PerformanceWindow::render(RenderEngine &render_engine) {

    const RenderBody header =
        RenderBodyBuilder()
            .shape(Shape::create_rectangle_data(HEADER_DIMENSION.x, HEADER_DIMENSION.y))
            .color(colors::BLUE)
            .position(HEADER_POSITION)
            .build();
    const RenderBody body =
        RenderBodyBuilder()
            .shape(Shape::create_rectangle_data(BODY_DIMENSION.x, BODY_DIMENSION.y))
            .color(colors::RED)
            .position(BODY_POSITION)
            .build();

    const auto components = {std::cref(header), std::cref(body)};

    auto m_menu = ui::Menu();
    auto m_ui = ui::UI(m_menu);
    m_ui.add_text_box(
        "HEADER",
        ui::TextBox("PERFORMANCE", ui::ElementProperties{.container.center = HEADER_TEXT,
                                                         .container.dimension =
                                                             glm::vec2(200.0f, 200.0f),
                                                         .font.color = colors::WHITE,
                                                         .font.size = 24,
                                                         .font.weight = 1.0f,
                                                         .font.sharpness = 1.0f}));

    // CONTINUE: Render text
    /*m_ui.add_text_box(FPS_TAG, ui::TextBox("FPS", ui::ElementProperties()));*/
    render_engine.render(components);

    auto ui_state = m_ui.get_state();
    render_engine.render_ui(ui_state);
}
