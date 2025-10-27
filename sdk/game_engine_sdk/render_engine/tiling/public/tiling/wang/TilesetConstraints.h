#pragma once

#include "tiling/traits.h"
#include "tiling/wang/TilesetIndex.h"
#include <optional>
#include <set>

namespace tiling::wang {

// Hasing function that supports 32-bit systems
template <WangEnumUint8 T> struct ConstraintHash {
    std::size_t operator()(const std::tuple<T, T, T, T, T> &t) const {
        std::size_t h = 0;
        h ^= std::hash<std::underlying_type_t<T>>{}(
                 static_cast<std::underlying_type_t<T>>(std::get<0>(t))) +
             0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<std::underlying_type_t<T>>{}(
                 static_cast<std::underlying_type_t<T>>(std::get<1>(t))) +
             0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<std::underlying_type_t<T>>{}(
                 static_cast<std::underlying_type_t<T>>(std::get<2>(t))) +
             0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<std::underlying_type_t<T>>{}(
                 static_cast<std::underlying_type_t<T>>(std::get<3>(t))) +
             0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<std::underlying_type_t<T>>{}(
                 static_cast<std::underlying_type_t<T>>(std::get<4>(t))) +
             0x9e3779b9 + (h << 6) + (h >> 2);
        return h;
    }
};

template <WangEnumUint8 T> struct TileConstraint {
    T type;
    std::set<T> north;
    std::set<T> east;
    std::set<T> south;
    std::set<T> west;
};

template <WangEnumUint8 T> class TilesetConstraints {
  private:
    size_t m_count;
    std::unordered_map<std::tuple<T, T, T, T, T>, TilesetIndex, ConstraintHash<T>>
        m_constraints;

  public:
    TilesetConstraints() = default;
    ~TilesetConstraints() = default;

    TilesetConstraints<T> &add_constraint(const TilesetIndex &&index,
                                          const TileConstraint<T> &&constraint) {

        for (const auto &n : constraint.north) {
            for (const auto &e : constraint.east) {
                for (const auto &s : constraint.south) {
                    for (const auto &w : constraint.west) {
                        m_constraints[std::make_tuple(constraint.type, n, e, s, w)] =
                            std::move(index);
                        m_count++;
                    }
                }
            }
        }
        return *this;
    }

    size_t count() const { return m_count; }

    std::optional<TilesetIndex> lookup_constraint(const T type, const T north,
                                                  const T east, const T south,
                                                  const T west) {
        return lookup_constraint(std::make_tuple(type, north, east, south, west));
    }

    std::optional<TilesetIndex>
    lookup_constraint(const std::tuple<T, T, T, T, T> &constraint) {
        auto it = m_constraints.find(constraint);
        if (it != m_constraints.end()) {
            return it->second;
        }
        return std::nullopt;
    }
};
} // namespace tiling::wang
