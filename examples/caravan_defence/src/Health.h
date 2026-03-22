#pragma once

#include "graphics_pipeline/geometry/GeometryRenderer.h"
#include "math/Matrix.h"
#include "util/colors.h"

enum class HealthBarType {
    Normal,
    EnergyShield,
    Armor,
};

struct HealthBarOpts {
    HealthBarType type = HealthBarType::Normal;
    float max_health = 100.0f;
};

class Health {
  private:
    static constexpr util::colors::Color health_color = util::colors::hex(0x39e504);
    static constexpr util::colors::Color energy_shield_color =
        util::colors::hex(0x02c0f9);
    static constexpr util::colors::Color armor_color =
        util::colors::rgb(0.2f, 0.2f, 0.2f);

    math::Vector2 m_health_bar_position = math::Vector2(0.0f, 0.0f);
    math::Vector2 m_health_bar_position_offset = math::Vector2(0.0f, 0.0f);
    math::Vector2 m_health_bar_size = math::Vector2(25.0f, 10.0f);

    struct HealthBar {
        graphics_pipeline::geometry::GeometrySBOHandle render_data_handle;
        float max = 100.0f;
        float current = 100.0f;
    };
    std::vector<HealthBar> m_health_pool;
    int m_active_health_bar = 0;

    graphics_pipeline::geometry::GeometryRenderer *m_geometry_renderer = nullptr;

    HealthBar create_health_bar(const HealthBarOpts &opts) {
        HealthBar health_bar{};
        health_bar.render_data_handle = m_geometry_renderer->request_render_slot();
        health_bar.max = opts.max_health;
        health_bar.current = opts.max_health;

        auto &instance = m_geometry_renderer->get_instance(health_bar.render_data_handle);
        instance.flags |=
            static_cast<uint32_t>(graphics_pipeline::geometry::GeometryShape::Rectangle);

        switch (opts.type) {
        case HealthBarType::Normal:
            instance.color = health_color;
            break;
        case HealthBarType::EnergyShield:
            instance.color = energy_shield_color;
            break;
        case HealthBarType::Armor:
            instance.color = armor_color;
            break;
        }

        return health_bar;
    }

  public:
    Health() = default;

    Health(graphics_pipeline::geometry::GeometryRenderer *geometry_renderer,
           const math::Vector2 &size, const HealthBarOpts opts = {})
        : m_geometry_renderer(geometry_renderer), m_health_bar_size(size) {
        m_health_pool.push_back(std::move(create_health_bar(opts)));
    }

    Health(const Health &) = delete;
    Health &operator=(const Health &) = delete;

    Health(Health &&) noexcept = default;
    Health &operator=(Health &&) noexcept = default;

    ~Health() {
        if (m_geometry_renderer != nullptr) {
            for (auto &bar : m_health_pool) {
                m_geometry_renderer->return_render_slot(bar.render_data_handle);
            }
            m_geometry_renderer = nullptr;
        }
    }

    bool is_dead() const { return m_health_pool[0].current <= 0.0f; }

    void set_health_bar_offset(const math::Vector2 &offset) {
        m_health_bar_position_offset = offset;
    }

    void set_health_bar_position(const math::Vector2 &position) {
        m_health_bar_position = position + m_health_bar_position_offset;
    }

    void update_health_bar() {
        DEBUG_ASSERT(m_geometry_renderer != nullptr,
                     "Error: Updated health bar but geometry renderer is nullptr.");

        for (size_t i = 0; i < m_health_pool.size(); i++) {
            HealthBar &bar = m_health_pool[i];

            auto &instance = m_geometry_renderer->get_instance(bar.render_data_handle);

            float percent_of_max = bar.current / bar.max;
            const math::Vector2 offset = math::Vector2(0.0f, m_health_bar_size.y() * i);
            instance.model_matrix =
                math::Matrix()
                    .translate(m_health_bar_position + offset)
                    .scale(m_health_bar_size.x() * percent_of_max, m_health_bar_size.y());
        }
    }

    float max_health() { return m_health_pool.back().max; }
    float current_health() { return m_health_pool.back().current; }

    void handle_incomming_damage(const float dmg) {
        m_health_pool[m_active_health_bar].current -= dmg;
        if (m_health_pool[m_active_health_bar].current <= 0.0f) {
            m_active_health_bar = fmax(0, m_active_health_bar - 1);
        }
    }

    void add_health_bar(const HealthBarOpts &opts) {
        m_health_pool.push_back(std::move(create_health_bar(opts)));
        m_active_health_bar = m_health_pool.size() - 1;
    }
};
