#pragma once

#include "tiling/NoiseMap.h"
#include "tiling/traits.h"
#include "tiling/wang/TilesetConstraints.h"
#include <iostream>
#include <vector>

namespace tiling::wang {

template <WangEnumUint8 T> class WangTiles {
  private:
    unsigned int m_grid_width;
    unsigned int m_grid_height;
    std::vector<T> m_tiles;

    TilesetConstraints<T> m_constraints;

    std::vector<std::optional<TilesetIndex>> m_tile_sprites;

    std::vector<T> assign_tile_types(const tiling::NoiseMap &noise_map,
                                     std::function<T(float)> tile_assign_rule) {
        std::vector<T> cell_types;
        cell_types.reserve(noise_map.size());
        for (auto c : noise_map.noise) {
            cell_types.push_back(tile_assign_rule(c));
        }
        return cell_types;
    }

    std::vector<std::optional<TilesetIndex>>
    assign_tile_sprites(std::vector<T> &cell_types) {
        std::vector<std::optional<TilesetIndex>> cell_sprites;
        cell_sprites.reserve(m_grid_width * m_grid_height);
        for (auto i = 0; i < cell_types.size(); i++) {
            const int x = i % m_grid_width;
            const int y = i / m_grid_width;

            const int left = x - 1;
            const int right = x + 1;
            const int top = i - m_grid_width;
            const int bottom = i + m_grid_width;

            const T current_type = cell_types[i];

            // For each cell, find the constraints based on bordering cells
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

            const std::tuple<T, T, T, T, T> constraints =
                std::tuple(current_type, top_constraint, right_constraint,
                           bottom_constraint, left_constraint);

            // TODO: A problem is that the outer most tiles will always have a "None"
            // constraint towards the edge of the map. Either I would need to have a
            // this "None" to a wildcard match or even easier, the outer most tiles
            // always have no texture.
            auto tileset_index = m_constraints.lookup_constraint(constraints);
            if (tileset_index) {
                cell_sprites.push_back(tileset_index.value());
            } else {
                cell_sprites.push_back(std::nullopt);
            }
        }
        return cell_sprites;
    }

  public:
    WangTiles() = default;
    ~WangTiles() = default;
    WangTiles(const tiling::NoiseMap &noise_map, std::function<T(float)> tile_assign_rule,
              TilesetConstraints<T> &&constraints)
        : m_grid_width(noise_map.width), m_grid_height(noise_map.height),
          m_constraints(std::move(constraints)) {
        m_tiles = assign_tile_types(std::move(noise_map), std::move(tile_assign_rule));
        m_tile_sprites = assign_tile_sprites(m_tiles);
    }

    unsigned int width() { return m_grid_width; }
    unsigned int height() { return m_grid_height; }

    void print_cell_type() {
        for (auto i = 0; i < m_tiles.size(); i++) {
            std::cout << m_tiles[i] << " ";
            if ((i + 1) % m_grid_width == 0) {
                std::cout << std::endl;
            }
        }
        std::cout << std::endl;
    }

    std::optional<TilesetIndex> lookup_tile(const unsigned int x, const unsigned int y) {
        return m_tile_sprites[y * m_grid_width + x];
    }
};
} // namespace tiling::wang
