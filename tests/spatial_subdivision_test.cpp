#include "physics_engine/RigidBody.h"
#include "physics_engine/broadphase/SpatialSubdivision.cpp"
#include "physics_engine/broadphase/SpatialSubdivision.h"
#include "test_utils.h"
#include <cstdint>
#include <gtest/gtest.h>

TEST(SpatialSubdivisionTest, TestNoCollisionCandidatesShouldExist) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(-200.0f, 0.0, 0.0))
                                 .velocity(glm::vec3(2.0, 0.0, 0.0))
                                 .mass(1.0)
                                 .angular_velocity(glm::radians(0.0))
                                 .shape(Shape::create_rectangle_data(100.0, 100.0))
                                 .build();
    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(200.0f, 0.0, 0.0))
                                 .velocity(glm::vec3(0.0, 0.0, 0.0))
                                 .mass(1.0)
                                 .angular_velocity(glm::radians(0.0))
                                 .shape(Shape::create_rectangle_data(50.0, 200.0))
                                 .build();
    const std::vector<RigidBody> bodies = {body_a, body_b};

    BoundingVolumes bounding_volumes = create_bounding_volumes(bodies);
    float cell_width = bounding_volumes.largest_radius * 2.0;
    EXPECT_NEAR(std::sqrt(50.0 / 2.0 * 50.0 / 2.0 + 200.0 / 2.0 * 200.0 / 2.0) * 1.41 *
                    2.0,
                cell_width, MAX_DIFF);

    auto [control_bits, cell_volumes] =
        create_cell_volumes(bounding_volumes.volumes, cell_width);

    SpatialSubdivision broadphase = SpatialSubdivision();
    auto collision_candidates = broadphase.collision_detection(bodies);
    std::cout << collision_candidates << std::endl;
    EXPECT_EQ(0, collision_candidates.pass1.size());
    EXPECT_EQ(0, collision_candidates.pass2.size());
    EXPECT_EQ(0, collision_candidates.pass3.size());
    EXPECT_EQ(0, collision_candidates.pass4.size());
}

TEST(SpatialSubdivisionTest, NoCollisionCandidatesShouldExistTestBoundingVolumes) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(-200.0f, 0.0, 0.0))
                                 .velocity(glm::vec3(2.0, 0.0, 0.0))
                                 .mass(1.0)
                                 .angular_velocity(glm::radians(0.0))
                                 .shape(Shape::create_rectangle_data(100.0, 100.0))
                                 .build();
    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(200.0f, 0.0, 0.0))
                                 .velocity(glm::vec3(0.0, 0.0, 0.0))
                                 .mass(1.0)
                                 .angular_velocity(glm::radians(0.0))
                                 .shape(Shape::create_rectangle_data(100.0, 100.0))
                                 .build();
    const std::vector<RigidBody> bodies = {body_a, body_b};

    const auto pos_diff = body_a.position - body_b.position;

    BoundingVolumes bounding_volumes = create_bounding_volumes(bodies);
    float cell_width = bounding_volumes.largest_radius * 2.0;
    EXPECT_NEAR(99.7021f, bounding_volumes.largest_radius, MAX_DIFF);

    EXPECT_LE(bounding_volumes.largest_radius, bounding_volumes.volumes[0].center.x);
    EXPECT_LE(bounding_volumes.largest_radius, bounding_volumes.volumes[0].center.y);
    EXPECT_LE(bounding_volumes.largest_radius, bounding_volumes.volumes[0].center.z);

    EXPECT_LE(bounding_volumes.largest_radius, bounding_volumes.volumes[1].center.x);
    EXPECT_LE(bounding_volumes.largest_radius, bounding_volumes.volumes[1].center.y);
    EXPECT_LE(bounding_volumes.largest_radius, bounding_volumes.volumes[1].center.z);

    const auto output_pos_diff =
        bounding_volumes.volumes[0].center - bounding_volumes.volumes[1].center;
    EXPECT_EQ(pos_diff, output_pos_diff);
}

TEST(SpatialSubdivisionCreateCellVolumeTest, TestObjectWithHomeInType2AndOverlapTLeft) {
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

TEST(SpatialSubdivisionCreateCellVolumeTest, TestObjectWithHomeInType3AndOverlapTop) {
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

TEST(SpatialSubdivisionCreateCellVolumeTest,
     TestObjectWithHomeInType4AndOverlapTopAndLeft) {
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

TEST(SpatialSubdivisionCreateCellVolumeTest,
     TestObjectWithHomeInType4AndOverlapLeftTopAndTopLeft) {
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

TEST(SpatialSubdivisionCreateCellVolumeTest, TestObjectWithHomeInType2AndOverlapRight) {
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

TEST(SpatialSubdivisionCreateCellVolumeTest, TestObjectWithHomeInType4AndOverlapTop) {
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

TEST(SpatialSubdivisionCreateCellVolumeTest,
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

TEST(SpatialSubdivisionCountVolumesPerCellTest, TestCoundVolumesPerCellTransitions) {
    std::vector<CellVolume> volumes{
        CellVolume{.x = 0, .y = 0, .z = 0, .cell_type = CellType::Home, .volume_id = 0},
        CellVolume{
            .x = 0, .y = 0, .z = 0, .cell_type = CellType::Phantom, .volume_id = 1},
        CellVolume{.x = 1, .y = 0, .z = 0, .cell_type = CellType::Home, .volume_id = 1},
        CellVolume{
            .x = 1, .y = 0, .z = 0, .cell_type = CellType::Phantom, .volume_id = 0},
    };

    auto output = count_volumes_per_cell(volumes);

    EXPECT_EQ(2, output.size());
    EXPECT_EQ(std::tuple(0, 2), output[0]);
    EXPECT_EQ(std::tuple(2, 2), output[1]);
}

TEST(SpatialSubdivisionCountVolumesPerCellTest,
     TestCoundVolumesPerCellLastVolumeIncluded) {
    std::vector<CellVolume> volumes{
        CellVolume{.x = 0, .y = 0, .z = 0, .cell_type = CellType::Home, .volume_id = 0},
        CellVolume{
            .x = 0, .y = 0, .z = 0, .cell_type = CellType::Phantom, .volume_id = 1},
        CellVolume{.x = 1, .y = 0, .z = 0, .cell_type = CellType::Home, .volume_id = 1},
        CellVolume{
            .x = 1, .y = 0, .z = 0, .cell_type = CellType::Phantom, .volume_id = 0},
        CellVolume{
            .x = 0, .y = 1, .z = 0, .cell_type = CellType::Phantom, .volume_id = 0},
    };

    auto output = count_volumes_per_cell(volumes);

    EXPECT_EQ(3, output.size());
    EXPECT_EQ(std::tuple(0, 2), output[0]);
    EXPECT_EQ(std::tuple(2, 2), output[1]);
    EXPECT_EQ(std::tuple(4, 1), output[2]);
}

TEST(SpatialSubdivisionCountVolumesPerCellTest, TestCoundVolumesPerCell3Objects) {
    std::vector<CellVolume> volumes{
        CellVolume{.x = 0, .y = 0, .z = 0, .cell_type = CellType::Home, .volume_id = 2},
        CellVolume{.x = 1, .y = 0, .z = 0, .cell_type = CellType::Home, .volume_id = 1},
        CellVolume{
            .x = 1, .y = 0, .z = 0, .cell_type = CellType::Phantom, .volume_id = 0},
        CellVolume{
            .x = 0, .y = 1, .z = 0, .cell_type = CellType::Phantom, .volume_id = 0},
        CellVolume{.x = 1, .y = 1, .z = 0, .cell_type = CellType::Home, .volume_id = 0},
        CellVolume{
            .x = 1, .y = 1, .z = 0, .cell_type = CellType::Phantom, .volume_id = 1},
    };

    auto output = count_volumes_per_cell(volumes);

    EXPECT_EQ(4, output.size());
    EXPECT_EQ(std::tuple(0, 1), output[0]);
    EXPECT_EQ(std::tuple(1, 2), output[1]);
    EXPECT_EQ(std::tuple(3, 1), output[2]);
    EXPECT_EQ(std::tuple(4, 2), output[3]);
}

TEST(SpatialSubdivisionSkipNarrowCheckTest,
     TestTwoObjectsWithDifferentHomeCellsButShareAllBoundingCellsDuringPass1ExpectFalse) {
    const uint8_t pass_num = 1;
    const ControlBits ctrl_a = 0b0010'0101;
    const ControlBits ctrl_b = 0b0000'0101;
    EXPECT_FALSE(can_we_skip_narrow_collision_check(pass_num, ctrl_a, ctrl_b));
}

TEST(SpatialSubdivisionSkipNarrowCheckTest,
     TestTwoObjectsWithDifferentHomeCellsButShareAllBoundingCellsDuringPass3ExpectTrue) {
    const uint8_t pass_num = 3;
    const ControlBits ctrl_a = 0b0010'0101;
    const ControlBits ctrl_b = 0b0000'0101;
    EXPECT_TRUE(can_we_skip_narrow_collision_check(pass_num, ctrl_a, ctrl_b));
}

TEST(SpatialSubdivisionSkipNarrowCheckTest,
     TestTwoObjectsWithDifferentHomeCellsButShareAllBoundingCellsDuringPass3ExpectFalse) {
    const uint8_t pass_num = 3;
    const ControlBits ctrl_a = 0b0011'1010;
    const ControlBits ctrl_b = 0b0010'1010;
    EXPECT_FALSE(can_we_skip_narrow_collision_check(pass_num, ctrl_a, ctrl_b));
}

TEST(SpatialSubdivisionSkipNarrowCheckTest,
     TestTwoObjectsWithDifferentHomeCellsButShareAllBoundingCellsDuringPass4ExpectTrue) {
    const uint8_t pass_num = 4;
    const ControlBits ctrl_a = 0b0011'1100;
    const ControlBits ctrl_b = 0b0010'1100;
    EXPECT_TRUE(can_we_skip_narrow_collision_check(pass_num, ctrl_a, ctrl_b));
}

TEST(
    SpatialSubdivisionSkipNarrowCheckTest,
    TestTwoObjectsWithDifferentHomeCellsButShareSubsetOfCellTypesDuringPass1ExpectFalse) {
    const uint8_t pass_num = 1;
    const ControlBits ctrl_a = 0b0010'0101;
    const ControlBits ctrl_b = 0b0000'0001;
    EXPECT_FALSE(can_we_skip_narrow_collision_check(pass_num, ctrl_a, ctrl_b));
}

TEST(SpatialSubdivisionSkipNarrowCheckTest,
     TestTwoObjectsWithDifferentHomeCellsButShareSubsetOfCellTypesDuringPass3ExpectTrue) {
    const uint8_t pass_num = 3;
    const ControlBits ctrl_a = 0b0010'0101;
    const ControlBits ctrl_b = 0b0000'0001;
    EXPECT_TRUE(can_we_skip_narrow_collision_check(pass_num, ctrl_a, ctrl_b));
}

TEST(
    SpatialSubdivisionSkipNarrowCheckTest,
    TestTwoObjectsWithDifferentHomeCellsAndNeitherHomeCellAmongCommonPhantomCellsDuringPass1ExpectFalse) {
    const uint8_t pass_num = 1;
    const ControlBits ctrl_a = 0b0001'0011;
    const ControlBits ctrl_b = 0b0010'0101;
    EXPECT_FALSE(can_we_skip_narrow_collision_check(pass_num, ctrl_a, ctrl_b));
}
