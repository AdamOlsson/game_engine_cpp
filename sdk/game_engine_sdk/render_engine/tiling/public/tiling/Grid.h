#pragma once

#include <cstddef>
#include <vector>

namespace tiling {

template <typename T> class Grid {
  private:
    const size_t m_grid_width;
    const size_t m_grid_height;

    std::vector<T> m_cells;

  public:
    Grid(const size_t width, const size_t height)
        : m_grid_width(width), m_grid_height(height) {
        m_cells.resize(m_grid_width * m_grid_height);
    }

    T &get_cell(const size_t x, const size_t y) { return m_cells[y * m_grid_width + x]; }

    const T &get_cell(const size_t x, const size_t y) const {
        return m_cells[y * m_grid_width + x];
    }

    T &get_cell(const size_t xy) { return m_cells[xy]; }
    const T &get_cell(const size_t xy) const { return m_cells[xy]; }

    void set_cell(const size_t x, const size_t y, T &value) {
        m_cells[y * m_grid_width + x] = value;
    }

    void set_cell(const size_t x, const size_t y, T &&value) {
        m_cells[y * m_grid_width + x] = std::move(value);
    }

    T &operator[](size_t i) { return m_cells[i]; }
    const T &operator[](size_t i) const { return m_cells[i]; }

    size_t width() { return m_grid_width; }
    size_t height() { return m_grid_height; }

    auto begin() { return m_cells.begin(); }
    auto end() { return m_cells.end(); }
    auto begin() const { return m_cells.begin(); }
    auto end() const { return m_cells.end(); }
    auto cbegin() const { return m_cells.cbegin(); }
    auto cend() const { return m_cells.cend(); }
};

} // namespace tiling
