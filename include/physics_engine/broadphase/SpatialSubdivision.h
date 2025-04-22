#pragma once

#include "physics_engine/RigidBody.h"
#include <ostream>
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

    /// Returns the total number of collision across all 4 passes
    size_t size() { return pass1.size() + pass2.size() + pass3.size() + pass4.size(); }
};

std::ostream &operator<<(std::ostream &os, const CollisionCandidatePair &ccp);
std::ostream &operator<<(std::ostream &os, const CollisionCandidates &cc);
std::ostream &operator<<(std::ostream &os, const SpatialSubdivisionResult &res);

class SpatialSubdivision {
  private:
    SpatialSubdivisionResult
    create_passes(const std::vector<std::tuple<size_t, size_t>> &cell_volume_count,
                  const std::vector<CellVolume> &cell_volumes,
                  const std::vector<ControlBits> &control_bits);

  public:
    SpatialSubdivision() = default;
    ~SpatialSubdivision() = default;

    SpatialSubdivisionResult collision_detection(const std::vector<RigidBody> &bodies);
};
