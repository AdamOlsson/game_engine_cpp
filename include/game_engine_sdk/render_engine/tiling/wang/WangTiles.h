#pragma once

#include "game_engine_sdk/render_engine/UVWT.h"
#include "game_engine_sdk/render_engine/tiling/NoiseMap.h"
#include "game_engine_sdk/traits.h"
#include "glm/glm.hpp"
#include <vector>

namespace tiling {

template <typename T>
concept EnumUint8 = std::is_enum_v<T> &&
                    std::is_same_v<std::underlying_type_t<T>, uint8_t> && Printable<T>;

template <EnumUint8 T> struct ConstraintHash {
    std::size_t operator()(const std::tuple<T, T, T, T> &t) const {
        return (static_cast<size_t>(std::get<0>(t)) << 24) |
               (static_cast<size_t>(std::get<1>(t)) << 16) |
               (static_cast<size_t>(std::get<2>(t)) << 8) |
               (static_cast<size_t>(std::get<3>(t)));
    }
};

template <EnumUint8 T> class WangTiles {
  private:
    unsigned int m_grid_width;
    unsigned int m_grid_height;
    std::vector<T> m_cells;

    std::unordered_map<std::tuple<T, T, T, T>, glm::vec4, ConstraintHash<T>>
        m_tileset_constraints;

    std::vector<UVWT> m_cell_sprites;

    std::vector<T> assign_cell_types(const tiling::NoiseMap &noise_map,
                                     std::function<T(float)> tile_assign_rule) {
        std::vector<T> cell_types;
        cell_types.reserve(noise_map.size());
        for (auto c : noise_map.noise) {
            cell_types.push_back(tile_assign_rule(c));
        }
        return cell_types;
    }

    std::vector<UVWT> assign_cell_sprites(std::vector<T> &cell_types) {
        std::vector<UVWT> cell_sprites;
        cell_sprites.reserve(m_grid_width * m_grid_height);
        for (auto i = 0; i < cell_types.size(); i++) {
            const int x = i % m_grid_width;
            const int y = i / m_grid_width;

            const int current = i;
            const int left = x - 1;
            const int right = x + 1;
            const int top = current - m_grid_width;
            const int bottom = current + m_grid_width;

            // For each cell, find the constraints based on bordering cells
            // TODO: How can I use the "none" value from the template enum?
            T left_constraint = static_cast<T>(0xFF);
            if (left >= 0) {
                left_constraint = cell_types[i - 1];
            }

            T top_constraint = static_cast<T>(0xFF);
            if (top >= 0) {
                top_constraint = cell_types[top];
            }

            T right_constraint = static_cast<T>(0xFF);
            if (right < m_grid_width) {
                right_constraint = cell_types[i + 1];
            }

            T bottom_constraint = static_cast<T>(0xFF);
            if (bottom < m_grid_width * m_grid_height) {
                bottom_constraint = cell_types[bottom];
            }

            const std::tuple<T, T, T, T> constraints = std::tuple(
                top_constraint, right_constraint, bottom_constraint, left_constraint);

            // TODO: A problem is that the outer most tiles will always have a "None"
            // constraint towards the edge of the map. Either I would need to have a
            // this "None" to a wildcard match or even easier, the outer most tiles
            // always have no texture.
            /*logger::debug(i, ": looking for constraint ", constraints);*/
            if (m_tileset_constraints.find(constraints) != m_tileset_constraints.end()) {
                cell_sprites.push_back(m_tileset_constraints[constraints]);

            } else {
                // TODO: Create some wrapper around uvwt coordinates have this be "no
                // uvwt"
                cell_sprites.push_back(glm::vec4(-1.0f));
            }
        }
        return cell_sprites;
    }

  public:
    WangTiles() = default;
    ~WangTiles() = default;
    WangTiles(const tiling::NoiseMap &noise_map, std::function<T(float)> tile_assign_rule,
              std::unordered_map<std::tuple<T, T, T, T>, glm::vec4, ConstraintHash<T>>
                  &&tileset_constraints)
        : m_grid_width(noise_map.width), m_grid_height(noise_map.height),
          m_tileset_constraints(std::move(tileset_constraints)) {
        m_cells = assign_cell_types(std::move(noise_map), std::move(tile_assign_rule));
        m_cell_sprites = assign_cell_sprites(m_cells);
    }

    unsigned int width() { return m_grid_width; }
    unsigned int height() { return m_grid_height; }

    void print_cell_type() {
        for (auto i = 0; i < m_cells.size(); i++) {
            std::cout << m_cells[i] << " ";
            if ((i + 1) % m_grid_width == 0) {
                std::cout << std::endl;
            }
        }
        std::cout << std::endl;
    }

    // Temporary?
    std::vector<T> get_cells() { return m_cells; }
    std::vector<UVWT> get_cell_uvwt() { return m_cell_sprites; }
};
} // namespace tiling
