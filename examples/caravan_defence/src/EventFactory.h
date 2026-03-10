#pragma once

// forward declarations
#include "GameState.h"
#include "camera/Camera.h"
#include "graphics_pipeline/geometry/GeometryRenderer.h"
class Event;

class EventFactory {
  public:
    static Event
    my_first_event(graphics_pipeline::geometry::GeometryRenderer *geom_renderer,
                   graphics_pipeline::text::TextRenderer *text_renderer,
                   const camera::Camera2D &camera, GameState &game_state);
};
