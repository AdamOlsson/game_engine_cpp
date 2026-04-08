#pragma once

#include "../events/Event.h"
#include "graphics_pipeline/geometry/GeometryPipelineSBO.h"
#include "graphics_pipeline/geometry/GeometryRenderer2.h"
#include "graphics_pipeline/text/TextRenderer2.h"
#include "state_machine/StateTransition.h"

struct GameState;

class EventState {
  private:
    graphics_pipeline::text::TextRenderer2 *m_text_renderer2 = nullptr;
    graphics_pipeline::geometry::GeometryRenderer2 *m_geometry_renderer = nullptr;

    std::optional<Event> event = std::nullopt;
    std::optional<util::StateTransition> m_pending_transition = std::nullopt;

  public:
    EventState() = default;

    EventState(graphics_pipeline::text::TextRenderer2 *text_renderer2,
               graphics_pipeline::geometry::GeometryRenderer2 *geom_renderer)
        : m_geometry_renderer(geom_renderer), m_text_renderer2(text_renderer2) {}

    EventState(const EventState &) = delete;
    EventState(EventState &&) = default;
    EventState &operator=(const EventState &) = delete;
    EventState &operator=(EventState &&) = default;

    void on_enter(GameState &game_state);
    void on_exit(GameState &game_state);
    util::StateTransition update(const float dt, GameState &game_state);

    std::vector<font::TextFormat> get_ui_text_format_render_data() {
        std::vector<font::TextFormat> text_format;
        std::vector<font::Text> text;
        if (event.has_value()) {
            event->get_text_render_data(text_format, text);
        }
        return text_format;
    }

    std::vector<font::Text> get_ui_text_render_data() {
        std::vector<font::TextFormat> text_format;
        std::vector<font::Text> text;
        if (event.has_value()) {
            event->get_text_render_data(text_format, text);
        }
        return text;
    }

    std::vector<graphics_pipeline::geometry::GeometryPipelineSBO>
    get_ui_geometry_render_data() {
        std::vector<graphics_pipeline::geometry::GeometryPipelineSBO> geometry_data;
        if (event.has_value()) {
            event->get_render_data(geometry_data);
        }
        return geometry_data;
    }

    void set_pending_transition(util::StateTransition transition) {
        m_pending_transition = transition;
    }
};
