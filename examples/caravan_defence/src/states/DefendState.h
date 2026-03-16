#pragma once

#include "camera/Camera.h"
#include "graphics_pipeline/geometry/GeometryRenderer.h"
#include "graphics_pipeline/quad/QuadRenderer.h"
#include "state_machine/StateTransition.h"
#include <vector>

struct GameState;

class DefendState {
  private:
    graphics_pipeline::quad::QuadRenderer *m_quad_renderer = nullptr;
    graphics_pipeline::geometry::GeometryRenderer *m_geometry_renderer = nullptr;

    constexpr void spawn_group_of_enemies(GameState &game_state);

    std::optional<size_t> find_caravan_cart(GameState &game_state,
                                            const camera::WorldPoint2D &point);

    template <typename T> void update_all(const float dt, std::vector<T> &vec) {
        for (T &t : vec) {
            t.update(dt);
        }
    }

  public:
    DefendState() = default;

    DefendState(graphics_pipeline::quad::QuadRenderer *quad_renderer,
                graphics_pipeline::geometry::GeometryRenderer *geom_renderer)
        : m_quad_renderer(quad_renderer), m_geometry_renderer(geom_renderer) {}

    DefendState(DefendState &&) noexcept = default;
    DefendState(const DefendState &) = delete;
    DefendState &operator=(DefendState &&) = default;
    DefendState &operator=(const DefendState &) = delete;

    void on_enter(GameState &game_state);
    void on_exit(GameState &game_state);
    util::StateTransition update(const float dt, GameState &game_state);
};
