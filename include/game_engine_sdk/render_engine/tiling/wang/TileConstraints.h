#pragma once

#include "game_engine_sdk/render_engine/tiling/wang/WangTiles.h"
#include <set>
#include <unordered_map>

template <typename T> struct TileConstaint {
    std::set<T> north;
    std::set<T> east;
    std::set<T> south;
    std::set<T> west;
};

/*class TilesetConstraint {*/
/*  private:*/
/*  public:*/
/*    std::unordered_map<std::tuple<tiling::CellType, tiling::CellType,
 * tiling::CellType,*/
/*                                  tiling::CellType>,*/
/*                       glm::vec4, tiling::ConstraintHash>*/
/*        constraints;*/
/**/
/*    TilesetConstraint() = default;*/
/*    ~TilesetConstraint() = default;*/
/*};*/
