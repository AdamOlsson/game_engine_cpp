#pragma once

#include "../events/Event.h"
#include "DefendState.h"
#include "graphics_pipeline/geometry/GeometryRenderer.h"
#include "graphics_pipeline/text/TextRenderer.h"
#include "state_machine/StateTransition.h"

struct GameState;

class EventState {
  private:
    std::optional<util::StateTransition> m_pending_transition = std::nullopt;

  public:
    // Not happy with this being public, but works for now. Will probably have to be
    // solved by having state specific rendering but how that would work is not clear yet.
    std::optional<Event> event = std::nullopt;

    EventState() = default;

    graphics_pipeline::text::TextRenderer *m_text_renderer = nullptr;
    graphics_pipeline::geometry::GeometryRenderer *m_geometry_renderer = nullptr;

    EventState(graphics_pipeline::text::TextRenderer *text_renderer,
               graphics_pipeline::geometry::GeometryRenderer *geom_renderer)
        : m_text_renderer(text_renderer), m_geometry_renderer(geom_renderer) {}

    EventState(const EventState &) = delete;
    EventState(EventState &&) = default;
    EventState &operator=(const EventState &) = delete;
    EventState &operator=(EventState &&) = default;

    void on_enter(GameState &game_state);
    void on_exit(GameState &game_state);
    util::StateTransition update(const float dt, GameState &game_state);

    void set_pending_transition(util::StateTransition transition) {
        m_pending_transition = transition;
    }
};
