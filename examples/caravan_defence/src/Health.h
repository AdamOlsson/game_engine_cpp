#pragma once

#include "graphics_pipeline/geometry/GeometryPipelineSBO.h"
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
        HealthBarType type;
        float max = 100.0f;
        float current = 100.0f;
    };
    std::vector<HealthBar> m_health_pool;
    std::vector<graphics_pipeline::geometry::GeometryPipelineSBO> m_render_data;

    int m_active_health_bar = 0;

    HealthBar create_health_bar(const HealthBarOpts &opts) {
        HealthBar health_bar{};
        health_bar.type = opts.type;
        health_bar.max = opts.max_health;
        health_bar.current = opts.max_health;
        return health_bar;
    }

    graphics_pipeline::geometry::GeometryPipelineSBO
    create_health_bar_render_data(const HealthBarType &type) {
        graphics_pipeline::geometry::GeometryPipelineSBO render_data{};
        render_data.flags |=
            static_cast<uint32_t>(graphics_pipeline::geometry::GeometryShape::Rectangle);
        switch (type) {
        case HealthBarType::Normal:
            render_data.color = health_color;
            break;
        case HealthBarType::EnergyShield:
            render_data.color = energy_shield_color;
            break;
        case HealthBarType::Armor:
            render_data.color = armor_color;
            break;
        }
        return render_data;
    }

  public:
    Health() = default;

    Health(const math::Vector2 &size, const HealthBarOpts opts = {})
        : m_health_bar_size(size) {
        m_health_pool.push_back(create_health_bar(opts));
        m_render_data.push_back(create_health_bar_render_data(opts.type));
    }

    Health(const Health &) = delete;
    Health &operator=(const Health &) = delete;

    Health(Health &&) noexcept = default;
    Health &operator=(Health &&) noexcept = default;

    ~Health() {}

    bool is_dead() const { return m_health_pool[0].current <= 0.0f; }

    void set_health_bar_offset(const math::Vector2 &offset) {
        m_health_bar_position_offset = offset;
    }

    void set_health_bar_position(const math::Vector2 &position) {
        m_health_bar_position = position + m_health_bar_position_offset;
    }

    void update_health_bar() {
        for (size_t i = 0; i < m_health_pool.size(); i++) {
            HealthBar &bar = m_health_pool[i];
            auto &bar_render_data = m_render_data[i];

            float percent_of_max = bar.current / bar.max;
            const math::Vector2 offset = math::Vector2(0.0f, m_health_bar_size.y() * i);
            bar_render_data.model_matrix =
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
        m_render_data.push_back(create_health_bar_render_data(opts.type));
        m_active_health_bar = m_health_pool.size() - 1;
    }

    HealthBarType get_active_health_bar_type() const {
        return m_health_pool[m_active_health_bar].type;
    }

    const std::vector<graphics_pipeline::geometry::GeometryPipelineSBO> &
    get_render_data() const {
        return m_render_data;
    }
};
