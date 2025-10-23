#pragma once

#include "game_engine_sdk/render_engine/tiling/wang/WangTiles.h"
#include <unordered_map>
class TilesetConstraint {
  private:
  public:
    std::unordered_map<std::tuple<tiling::CellType, tiling::CellType, tiling::CellType,
                                  tiling::CellType>,
                       glm::vec4, tiling::ConstraintHash>
        constraints;

    TilesetConstraint() = default;
    ~TilesetConstraint() = default;
};
