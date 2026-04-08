#pragma once

#include "../events/Event.h"
#include "font/Font.h"
#include "graphics_pipeline/geometry/GeometryPipelineSBO.h"
#include "state_machine/StateTransition.h"

struct GameState;

class IntroState {
  public:
    IntroState() = default;

    IntroState(font::Font &font) : m_font(&font) {}

    IntroState(const IntroState &) = delete;
    IntroState(IntroState &&) = default;
    IntroState &operator=(const IntroState &) = delete;
    IntroState &operator=(IntroState &&) = default;

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

  private:
    font::Font *m_font = nullptr;
    std::optional<Event> event = std::nullopt;
    std::optional<util::StateTransition> m_pending_transition = std::nullopt;

    Event create_intro_contract(GameState &game_state, IntroState &event_state,
                                font::Font *font);
};
