#include <gtest/gtest.h>

#include "tiling/wang/TilesetConstraints.h"
#include "tiling/wang/TilesetIndex.h"

enum class CellType : uint8_t {
    None,
    Wall,
    Grass,
};

inline std::ostream &operator<<(std::ostream &os, CellType type) {
    switch (type) {
    case CellType::None:
        return os << "CellType::None";
    case CellType::Wall:
        return os << "CellType::Wall";
    case CellType::Grass:
        return os << "CellType::Grass";
    }
}

TEST(WangTilesetConstraintTest, Test_AddSingleConstraint) {
    auto constraints = tiling::wang::TilesetConstraints<CellType>();
    constraints.add_constraint(tiling::wang::TilesetIndex(0, 0),
                               tiling::wang::TileConstraint<CellType>{
                                   .north = {CellType::Wall},
                                   .east = {CellType::Wall},
                                   .south = {CellType::Wall},
                                   .west = {CellType::Wall},
                               });
    auto tileset_index = constraints.lookup_sprite(CellType::Wall, CellType::Wall,
                                                   CellType::Wall, CellType::Wall);
    EXPECT_TRUE(tileset_index.has_value());
    ASSERT_EQ(tiling::wang::TilesetIndex(0, 0), tileset_index.value());
}
