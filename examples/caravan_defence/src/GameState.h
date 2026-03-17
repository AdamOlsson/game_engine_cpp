#pragma once

#include "Entity.h"
#include "events/Event.h"
#include <optional>
#include <random>

enum class GameMode {
    Playing,
    Paused,
    Event,
};

struct GameState {

    GameMode last_mode = GameMode::Playing;
    GameMode mode = GameMode::Playing;

    std::optional<Event> event;

    std::vector<entity::Entity> caravan;
    std::vector<entity::Entity> caravan_slots;
    std::vector<entity::Entity> guards;
    std::vector<entity::Entity> attacks;
    std::vector<entity::Entity> enemies;

    size_t time_elapsed_ms;
    std::optional<size_t> selected_guard = std::nullopt;

    camera::Camera2D camera;
    struct {
        std::optional<interface::ViewportPoint> click_point = std::nullopt;
        interface::ViewportPoint viewport_position = interface::ViewportPoint(1e6, 1e6);
        bool is_right_button_pressed = false;
    } cursor;

    struct {
        std::random_device device;
        std::mt19937 gen;
        void init() { gen = std::mt19937(device()); }
        float uniform(const float from, const float to) {
            std::uniform_real_distribution<float> dist(from, to);
            return dist(gen);
        }

        int uniform(const int from, const int to) {
            std::uniform_int_distribution<int> dist(from, to);
            return dist(gen);
        }
    } rng;
};
