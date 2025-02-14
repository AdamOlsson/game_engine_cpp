#include "render_engine/RenderBody.h"
#include "io.h"
#include "shape.h"
#include <ostream>

std::ostream &operator<<(std::ostream &os, const RenderBody &b) {
    os << "RenderBody( position: " << b.position << ", rotation: " << b.rotation
       << ", color: " << b.color << ", shape: " << b.shape << ")";
    return os;
}
