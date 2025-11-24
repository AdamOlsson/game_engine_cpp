#pragma once

#include <cstddef>
#include <vector>

/// @namespace tiling
/// Contains tile-based data structure utilities.
namespace tiling {

/// @class Grid
/// @brief A 2D grid container that stores elements in row-major order.
///
/// This is a template class that manages a contiguous 2D grid of elements of any type.
/// The grid uses row-major ordering where elements are stored in a flat vector and
/// accessed via 2D coordinates (x, y) or linear indices.
///
/// @tparam T The type of elements stored in the grid.
template <typename T> class Grid {
  private:
    size_t m_grid_width;    ///< The number of columns in the grid.
    size_t m_grid_height;   ///< The number of rows in the grid.
    std::vector<T> m_cells; ///< Flat storage for all grid cells in row-major order.

  public:
    /// @brief Default constructor.
    /// Creates an empty grid with no dimensions or cells.
    Grid() = default;

    /// @brief Constructs a grid with specified dimensions.
    ///
    /// @param width The number of columns in the grid.
    /// @param height The number of rows in the grid.
    ///
    /// All cells are default-initialized. The total number of cells is width * height.
    Grid(const size_t width, const size_t height)
        : m_grid_width(width), m_grid_height(height) {
        m_cells.resize(m_grid_width * m_grid_height);
    }

    /// @brief Constructs a grid from a 2D vector.
    ///
    /// @param grid A vector of vectors where each inner vector represents a row.
    ///
    /// The grid dimensions are inferred from the input. Width is determined by the
    /// size of the first row, and height is the number of rows. Elements are copied
    /// into row-major order in the internal flat vector.
    Grid(const std::vector<std::vector<T>> &grid)
        : m_grid_width(grid.size() > 0 ? grid[0].size() : 0), m_grid_height(grid.size()) {
        m_cells.reserve(m_grid_width * m_grid_height);
        for (auto r : grid) {
            for (auto c : r) {
                m_cells.push_back(c);
            }
        }
    }

    /// @brief Accesses a cell at the given 2D coordinates (read/write).
    ///
    /// @param x The column index.
    /// @param y The row index.
    /// @return Reference to the cell at position (x, y).
    T &get_cell(const size_t x, const size_t y) { return m_cells[y * m_grid_width + x]; }

    /// @brief Accesses a cell at the given 2D coordinates (read-only).
    ///
    /// @param x The column index.
    /// @param y The row index.
    /// @return Const reference to the cell at position (x, y).
    const T &get_cell(const size_t x, const size_t y) const {
        return m_cells[y * m_grid_width + x];
    }

    /// @brief Accesses a cell at a linear index (read/write).
    ///
    /// @param xy The linear index into the flat cell storage.
    /// @return Reference to the cell at the given index.
    T &get_cell(const size_t xy) { return m_cells[xy]; }

    /// @brief Accesses a cell at a linear index (read-only).
    ///
    /// @param xy The linear index into the flat cell storage.
    /// @return Const reference to the cell at the given index.
    const T &get_cell(const size_t xy) const { return m_cells[xy]; }

    /// @brief Sets a cell value at the given 2D coordinates (lvalue reference).
    ///
    /// @param x The column index.
    /// @param y The row index.
    /// @param value The new value to assign (passed by reference).
    void set_cell(const size_t x, const size_t y, T &value) {
        m_cells[y * m_grid_width + x] = value;
    }

    /// @brief Sets a cell value at the given 2D coordinates (rvalue reference).
    ///
    /// @param x The column index.
    /// @param y The row index.
    /// @param value The new value to assign (passed by rvalue reference for move
    /// semantics).
    void set_cell(const size_t x, const size_t y, T &&value) {
        m_cells[y * m_grid_width + x] = std::move(value);
    }

    /// @brief Subscript operator for accessing cells by linear index (read/write).
    ///
    /// @param i The linear index into the flat cell storage.
    /// @return Reference to the cell at index i.
    T &operator[](size_t i) { return m_cells[i]; }

    /// @brief Subscript operator for accessing cells by linear index (read-only).
    ///
    /// @param i The linear index into the flat cell storage.
    /// @return Const reference to the cell at index i.
    const T &operator[](size_t i) const { return m_cells[i]; }

    /// @brief Gets the width (number of columns) of the grid.
    /// @return The grid width.
    size_t width() { return m_grid_width; }

    /// @brief Gets the height (number of rows) of the grid.
    /// @return The grid height.
    size_t height() { return m_grid_height; }

    /// @brief Returns an iterator to the beginning of the cell storage.
    auto begin() { return m_cells.begin(); }

    /// @brief Returns an iterator to the end of the cell storage.
    auto end() { return m_cells.end(); }

    /// @brief Returns a const iterator to the beginning of the cell storage.
    auto begin() const { return m_cells.begin(); }

    /// @brief Returns a const iterator to the end of the cell storage.
    auto end() const { return m_cells.end(); }

    /// @brief Returns a const iterator to the beginning of the cell storage.
    auto cbegin() const { return m_cells.cbegin(); }

    /// @brief Returns a const iterator to the end of the cell storage.
    auto cend() const { return m_cells.cend(); }

    /// @brief Move constructor (defaulted).
    Grid(Grid &&other) noexcept = default;

    /// @brief Move assignment operator (defaulted).
    Grid &operator=(Grid &&other) noexcept = default;

    /// @brief Copy constructor (defaulted).
    Grid(const Grid &other) = default;

    /// @brief Copy assignment operator (defaulted).
    Grid &operator=(const Grid &other) = default;
};
} // namespace tiling
