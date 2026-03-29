#pragma once

#include "graphics_pipeline/geometry/GeometryRenderer2.h"
#include "graphics_pipeline/text/TextRenderer.h"
#include "graphics_pipeline/text/TextRenderer2.h"

struct GameState;
class Event;
class EventState;

class EventFactory {
  public:
    static Event
    my_first_event(graphics_pipeline::geometry::GeometryRenderer2 *geom_renderer,
                   graphics_pipeline::text::TextRenderer *text_renderer,
                   graphics_pipeline::text::TextRenderer2 *text_renderer2,
                   GameState &game_state, EventState &event_state);

    static Event
    the_scavenger_ambush(graphics_pipeline::geometry::GeometryRenderer2 *geom_renderer,
                         graphics_pipeline::text::TextRenderer *text_renderer,
                         GameState &game_state, EventState &event_state);

    static Event
    defectors_den(graphics_pipeline::geometry::GeometryRenderer2 *geom_renderer,
                  graphics_pipeline::text::TextRenderer *text_renderer,
                  GameState &game_state, EventState &event_state);
};
