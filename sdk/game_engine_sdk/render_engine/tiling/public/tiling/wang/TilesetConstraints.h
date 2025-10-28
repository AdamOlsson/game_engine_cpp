#pragma once

/*#include "logger/logger.h"*/
#include "logger/logger.h"
#include "tiling/traits.h"
#include "tiling/wang/TilesetIndex.h"
#include <iostream>
#include <optional>
#include <set>

namespace tiling::wang {

template <WangEnumUint8 T> struct TilesetTile {
    T type;
    struct TilesetTileConstraints {
        std::set<T> north;
        std::set<T> east;
        std::set<T> south;
        std::set<T> west;
    } constraints;
};

template <WangEnumUint8 T> struct TilesetKey {
    T type;
    T north;
    T east;
    T south;
    T west;

    TilesetKey(T type, T n, T e, T s, T w)
        : type(type), north(n), east(e), south(s), west(w) {}

    friend std::ostream &operator<<(std::ostream &os, const TilesetKey<T> &obj) {
        return os << "(" << obj.type << ", " << obj.north << ", " << obj.east << ", "
                  << obj.south << ", " << obj.west << ")";
    }

    bool operator==(const TilesetKey &other) const = default;
};

// Hasing function that supports 32-bit systems
template <WangEnumUint8 T> struct TilesetKeyHash {
    std::size_t operator()(const TilesetKey<T> &key) const {
        std::size_t h = 0;
        h ^= std::hash<std::underlying_type_t<T>>{}(
                 static_cast<std::underlying_type_t<T>>(key.type)) +
             0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<std::underlying_type_t<T>>{}(
                 static_cast<std::underlying_type_t<T>>(key.north)) +
             0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<std::underlying_type_t<T>>{}(
                 static_cast<std::underlying_type_t<T>>(key.east)) +
             0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<std::underlying_type_t<T>>{}(
                 static_cast<std::underlying_type_t<T>>(key.south)) +
             0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<std::underlying_type_t<T>>{}(
                 static_cast<std::underlying_type_t<T>>(key.west)) +
             0x9e3779b9 + (h << 6) + (h >> 2);
        return h;
    }
};

template <WangEnumUint8 T> class TilesetConstraints {
  private:
    size_t m_count;
    std::unordered_map<TilesetKey<T>, TilesetIndex, TilesetKeyHash<T>> m_constraints;

  public:
    TilesetConstraints() = default;
    ~TilesetConstraints() = default;

    TilesetConstraints<T> &add_constraint(const TilesetIndex &&index,
                                          const TilesetTile<T> &&tile) {

        for (const auto &n : tile.constraints.north) {
            for (const auto &e : tile.constraints.east) {
                for (const auto &s : tile.constraints.south) {
                    for (const auto &w : tile.constraints.west) {
                        const auto key = TilesetKey<T>(tile.type, n, e, s, w);
                        const auto it = m_constraints.find(key);
                        if (it != m_constraints.end()) {
                            const auto old_index = it->second;
                            logger::warning("Overwriting existing constraint ", key,
                                            " and tileset index ", old_index,
                                            " for new tileset index ", index);
                        }
                        m_constraints[key] = std::move(index);
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
        return lookup_constraint(TilesetKey<T>(type, north, east, south, west));
    }

    std::optional<TilesetIndex> lookup_constraint(const TilesetKey<T> &constraint) {
        auto it = m_constraints.find(constraint);
        if (it != m_constraints.end()) {
            return it->second;
        }
        return std::nullopt;
    }
};
} // namespace tiling::wang
