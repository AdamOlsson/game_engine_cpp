#pragma once
#include "game_engine_sdk/Dimension.h"
#include "game_engine_sdk/WorldPoint.h"

class Grid {
  private:
    Dimension m_cell_size;
    WorldPoint m_center_offset;

  public:
    Grid() = default;
    ~Grid() = default;

    Grid(Dimension cell_size, WorldPoint center = WorldPoint(0, 0));

    WorldPoint world_point_at(int x, int y);
    Dimension cell_dimension();
};
