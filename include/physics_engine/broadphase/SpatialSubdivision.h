#pragma once

#include "physics_engine/RigidBody.h"
#include <vector>

typedef std::tuple<size_t, size_t> CollisionCandidatePair;
typedef std::vector<CollisionCandidatePair> CollisionCandidates;
typedef u_int8_t ControlBits;

struct BoundingCircle;
struct CellVolume;

struct SpatialSubdivisionResult {
    std::vector<CollisionCandidates> pass1;
    std::vector<CollisionCandidates> pass2;
    std::vector<CollisionCandidates> pass3;
    std::vector<CollisionCandidates> pass4;
};

class SpatialSubdivision {
  private:
    SpatialSubdivisionResult
    create_passes(const std::vector<std::tuple<size_t, size_t>> &cell_volume_count,
                  const std::vector<CellVolume> &cell_volumes,
                  const std::vector<ControlBits> &control_bits);

  public:
    SpatialSubdivision() = default;
    ~SpatialSubdivision() = default;

    SpatialSubdivisionResult
    collision_detection(const std::vector<const RigidBody> &bodies);
};
