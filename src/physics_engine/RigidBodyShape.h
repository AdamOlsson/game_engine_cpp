#include "physics_engine/RigidBody.h"

class RigidBodyShape {
  public:
    virtual WorldPoint closest_point_on_body(const WorldPoint &) const = 0;
    virtual bool is_point_inside(const RigidBody &body,
                                 const WorldPoint &point) const = 0;
    virtual std::vector<glm::vec3> edges() const = 0;
    virtual std::vector<glm::vec3> vertices() const = 0;
    virtual std::vector<glm::vec3> normals() const = 0;
};
