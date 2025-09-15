#include "game_engine_sdk/render_engine/RenderBody.h"
#include "game_engine_sdk/io.h"
#include "game_engine_sdk/shape.h"
#include <ostream>

std::ostream &operator<<(std::ostream &os, const RenderBody &b) {
    os << "RenderBody( position: " << b.position << ", rotation: " << b.rotation
       << ", color: " << b.color << ", shape: " << b.shape << ")";
    return os;
}
