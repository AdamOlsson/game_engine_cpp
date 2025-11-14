#include "game_engine_sdk/render_engine/RenderBody.h"
#include "game_engine_sdk/shape.h"
#include "logger/io.h"
#include <ostream>

std::ostream &operator<<(std::ostream &os, const RenderBody &b) {
    os << "RenderBody( position: " << b.position << ", rotation: " << b.rotation
       << ", color: " << b.color << ", shape: " << b.shape << ")";
    return os;
}
