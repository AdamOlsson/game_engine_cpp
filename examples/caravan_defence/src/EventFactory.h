#pragma once

// forward declarations
#include "camera/Camera.h"
#include "graphics_pipeline/geometry/GeometryRenderer.h"
#include "graphics_pipeline/text/TextRenderer.h"

struct GameState;
class Event;
class EventState;

class EventFactory {
  public:
    static Event
    my_first_event(graphics_pipeline::geometry::GeometryRenderer *geom_renderer,
                   graphics_pipeline::text::TextRenderer *text_renderer,
                   const camera::Camera2D &camera, GameState &game_state,
                   EventState &event_state);
};
