#include "physics_engine/broadphase/SpatialSubdivision.cpp"
#include "physics_engine/broadphase/SpatialSubdivision.h"
#include <gtest/gtest.h>

TEST(SpatialSubdivisionTest, TestObjectWithHomeInType2AndOverlapTLeft) {
    float cell_width = 0.1f;
    std::vector<BoundingCircle> volumes = {
        BoundingCircle{.center = glm::vec3(0.11, 0.025f, 0.0f), .radius = 0.015f}};

    auto [control_bits, cell_volumes] = create_cell_volumes(volumes, cell_width);

    EXPECT_EQ(1, control_bits.size());
    EXPECT_EQ(0b0001'0011, control_bits[0]);
    EXPECT_EQ(2, cell_volumes.size());

    size_t index = 0;
    EXPECT_EQ(0, cell_volumes[index].volume_id);
    EXPECT_EQ(CellType::Home, cell_volumes[index].cell_type);
    EXPECT_EQ(1, cell_volumes[index].x);
    EXPECT_EQ(0, cell_volumes[index].y);
    EXPECT_EQ(0, cell_volumes[index].z);

    index = 1;
    EXPECT_EQ(0, cell_volumes[index].volume_id);
    EXPECT_EQ(CellType::Phantom, cell_volumes[index].cell_type);
    EXPECT_EQ(0, cell_volumes[index].x);
    EXPECT_EQ(0, cell_volumes[index].y);
    EXPECT_EQ(0, cell_volumes[index].z);
}

TEST(SpatialSubdivisionTest, TestObjectWithHomeInType3AndOverlapTop) {
    float cell_width = 0.1f;
    std::vector<BoundingCircle> volumes = {
        BoundingCircle{.center = glm::vec3(0.025f, 0.11f, 0.0f), .radius = 0.015f}};
    auto [control_bits, cell_volumes] = create_cell_volumes(volumes, cell_width);

    EXPECT_EQ(1, control_bits.size());
    EXPECT_EQ(0b0010'0101, control_bits[0]);
    EXPECT_EQ(2, cell_volumes.size());

    size_t index = 0;
    EXPECT_EQ(0, cell_volumes[index].volume_id);
    EXPECT_EQ(CellType::Home, cell_volumes[index].cell_type);
    EXPECT_EQ(0, cell_volumes[index].x);
    EXPECT_EQ(1, cell_volumes[index].y);
    EXPECT_EQ(0, cell_volumes[index].z);

    index = 1;
    EXPECT_EQ(0, cell_volumes[index].volume_id);
    EXPECT_EQ(CellType::Phantom, cell_volumes[index].cell_type);
    EXPECT_EQ(0, cell_volumes[index].x);
    EXPECT_EQ(0, cell_volumes[index].y);
    EXPECT_EQ(0, cell_volumes[index].z);
}

TEST(SpatialSubdivisionTest, TestObjectWithHomeInType4AndOverlapTopAndLeft) {
    float cell_width = 0.1f;
    std::vector<BoundingCircle> volumes = {
        BoundingCircle{.center = glm::vec3(0.11f, 0.11f, 0.0f), .radius = 0.0141f}};
    auto [control_bits, cell_volumes] = create_cell_volumes(volumes, cell_width);

    EXPECT_EQ(1, control_bits.size());
    EXPECT_EQ(0b0011'1110, control_bits[0]);
    EXPECT_EQ(3, cell_volumes.size());

    size_t index = 0;
    EXPECT_EQ(0, cell_volumes[index].volume_id);
    EXPECT_EQ(CellType::Home, cell_volumes[index].cell_type);
    EXPECT_EQ(1, cell_volumes[index].x);
    EXPECT_EQ(1, cell_volumes[index].y);
    EXPECT_EQ(0, cell_volumes[index].z);

    index = 1;
    EXPECT_EQ(0, cell_volumes[index].volume_id);
    EXPECT_EQ(CellType::Phantom, cell_volumes[index].cell_type);
    EXPECT_EQ(0, cell_volumes[index].x);
    EXPECT_EQ(1, cell_volumes[index].y);
    EXPECT_EQ(0, cell_volumes[index].z);

    index = 2;
    EXPECT_EQ(0, cell_volumes[index].volume_id);
    EXPECT_EQ(CellType::Phantom, cell_volumes[index].cell_type);
    EXPECT_EQ(1, cell_volumes[index].x);
    EXPECT_EQ(0, cell_volumes[index].y);
    EXPECT_EQ(0, cell_volumes[index].z);
}

TEST(SpatialSubdivisionTest, TestObjectWithHomeInType4AndOverlapLeftTopAndTopLeft) {
    float cell_width = 0.1f;
    std::vector<BoundingCircle> volumes = {
        BoundingCircle{.center = glm::vec3(0.11f, 0.11f, 0.0f), .radius = 0.02f}};
    auto [control_bits, cell_volumes] = create_cell_volumes(volumes, cell_width);

    EXPECT_EQ(1, control_bits.size());
    EXPECT_EQ(0b0011'1111, control_bits[0]);
    EXPECT_EQ(4, cell_volumes.size());

    size_t index = 0;
    EXPECT_EQ(0, cell_volumes[index].volume_id);
    EXPECT_EQ(CellType::Home, cell_volumes[index].cell_type);
    EXPECT_EQ(1, cell_volumes[index].x);
    EXPECT_EQ(1, cell_volumes[index].y);
    EXPECT_EQ(0, cell_volumes[index].z);

    index = 1;
    EXPECT_EQ(0, cell_volumes[index].volume_id);
    EXPECT_EQ(CellType::Phantom, cell_volumes[index].cell_type);
    EXPECT_EQ(0, cell_volumes[index].x);
    EXPECT_EQ(1, cell_volumes[index].y);
    EXPECT_EQ(0, cell_volumes[index].z);

    index = 2;
    EXPECT_EQ(0, cell_volumes[index].volume_id);
    EXPECT_EQ(CellType::Phantom, cell_volumes[index].cell_type);
    EXPECT_EQ(1, cell_volumes[index].x);
    EXPECT_EQ(0, cell_volumes[index].y);
    EXPECT_EQ(0, cell_volumes[index].z);

    index = 3;
    EXPECT_EQ(0, cell_volumes[index].volume_id);
    EXPECT_EQ(CellType::Phantom, cell_volumes[index].cell_type);
    EXPECT_EQ(0, cell_volumes[index].x);
    EXPECT_EQ(0, cell_volumes[index].y);
    EXPECT_EQ(0, cell_volumes[index].z);
}

TEST(SpatialSubdivisionTest, TestObjectWithHomeInType2AndOverlapRight) {
    float cell_width = 0.1f;
    std::vector<BoundingCircle> volumes = {
        BoundingCircle{.center = glm::vec3(0.39f, 0.025f, 0.0f), .radius = 0.02f}};
    auto [control_bits, cell_volumes] = create_cell_volumes(volumes, cell_width);

    EXPECT_EQ(1, control_bits.size());
    EXPECT_EQ(0b0001'0011, control_bits[0]);
    EXPECT_EQ(2, cell_volumes.size());

    size_t index = 0;
    EXPECT_EQ(0, cell_volumes[index].volume_id);
    EXPECT_EQ(CellType::Home, cell_volumes[index].cell_type);
    EXPECT_EQ(3, cell_volumes[index].x);
    EXPECT_EQ(0, cell_volumes[index].y);
    EXPECT_EQ(0, cell_volumes[index].z);

    index = 1;
    EXPECT_EQ(0, cell_volumes[index].volume_id);
    EXPECT_EQ(CellType::Phantom, cell_volumes[index].cell_type);
    EXPECT_EQ(4, cell_volumes[index].x);
    EXPECT_EQ(0, cell_volumes[index].y);
    EXPECT_EQ(0, cell_volumes[index].z);
}

TEST(SpatialSubdivisionTest, TestObjectWithHomeInType4AndOverlapTop) {
    float cell_width = 0.1f;
    std::vector<BoundingCircle> volumes = {
        BoundingCircle{.center = glm::vec3(0.375f, 0.11f, 0.0f), .radius = 0.02f}};
    auto [control_bits, cell_volumes] = create_cell_volumes(volumes, cell_width);
    EXPECT_EQ(1, control_bits.size());
    EXPECT_EQ(0b0011'1010, control_bits[0]);
    EXPECT_EQ(2, cell_volumes.size());

    size_t index = 0;
    EXPECT_EQ(0, cell_volumes[index].volume_id);
    EXPECT_EQ(CellType::Home, cell_volumes[index].cell_type);
    EXPECT_EQ(3, cell_volumes[index].x);
    EXPECT_EQ(1, cell_volumes[index].y);
    EXPECT_EQ(0, cell_volumes[index].z);

    index = 1;
    EXPECT_EQ(0, cell_volumes[index].volume_id);
    EXPECT_EQ(CellType::Phantom, cell_volumes[index].cell_type);
    EXPECT_EQ(3, cell_volumes[index].x);
    EXPECT_EQ(0, cell_volumes[index].y);
    EXPECT_EQ(0, cell_volumes[index].z);
}

TEST(SpatialSubdivisionTest,
     TestObjectWithHomeInType4AndOverlapWithTopRightAndRightAndTop) {
    float cell_width = 0.1f;
    std::vector<BoundingCircle> volumes = {
        BoundingCircle{.center = glm::vec3(0.39f, 0.11f, 0.0f), .radius = 0.02f}};
    auto [control_bits, cell_volumes] = create_cell_volumes(volumes, cell_width);
    EXPECT_EQ(1, control_bits.size());
    EXPECT_EQ(0b0011'1111, control_bits[0]);
    EXPECT_EQ(4, cell_volumes.size());

    size_t index = 0;
    EXPECT_EQ(0, cell_volumes[index].volume_id);
    EXPECT_EQ(CellType::Home, cell_volumes[index].cell_type);
    EXPECT_EQ(3, cell_volumes[index].x);
    EXPECT_EQ(1, cell_volumes[index].y);
    EXPECT_EQ(0, cell_volumes[index].z);

    index = 1;
    EXPECT_EQ(0, cell_volumes[index].volume_id);
    EXPECT_EQ(CellType::Phantom, cell_volumes[index].cell_type);
    EXPECT_EQ(4, cell_volumes[index].x);
    EXPECT_EQ(1, cell_volumes[index].y);
    EXPECT_EQ(0, cell_volumes[index].z);

    index = 2;
    EXPECT_EQ(0, cell_volumes[index].volume_id);
    EXPECT_EQ(CellType::Phantom, cell_volumes[index].cell_type);
    EXPECT_EQ(3, cell_volumes[index].x);
    EXPECT_EQ(0, cell_volumes[index].y);
    EXPECT_EQ(0, cell_volumes[index].z);

    index = 3;
    EXPECT_EQ(0, cell_volumes[index].volume_id);
    EXPECT_EQ(CellType::Phantom, cell_volumes[index].cell_type);
    EXPECT_EQ(4, cell_volumes[index].x);
    EXPECT_EQ(0, cell_volumes[index].y);
    EXPECT_EQ(0, cell_volumes[index].z);
}
