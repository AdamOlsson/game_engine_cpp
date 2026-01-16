#pragma once

#include <cstring>
#include <ostream>
namespace font {
enum FontFormat { TrueType, CFF, Type1, Unkown };
enum WindingOrder { Clockwise, CounterClockwise };
namespace font_format {

inline const char *to_string(enum FontFormat format) {
    switch (format) {
    case TrueType:
        return "TrueType";
    case CFF:
        return "CFF";
    case Type1:
        return "Type 1";
    default:
        return "Unknown";
    }
}

inline enum FontFormat from_string(const char *format) {
    if (format == nullptr) {
        return Unkown;
    }

    if (strcmp(format, "TrueType") == 0) {
        return TrueType;
    } else if (strcmp(format, "CFF") == 0) {
        return CFF;
    } else if (strcmp(format, "Type 1") == 0) {
        return Type1;
    }

    return Unkown; // Default fallback
}
} // namespace font_format

namespace winding_order {

inline WindingOrder from_font_format(const FontFormat format) {
    switch (format) {
    case TrueType:
        return Clockwise;
    case CFF:
    case Type1:
        return CounterClockwise;
    default:
        return Clockwise; // Default fallback
    }
}

inline const char *to_string(enum WindingOrder order) {
    switch (order) {
    case Clockwise:
        return "Clockwise";
    case CounterClockwise:
        return "CounterClockwise";
    default:
        return "Unknown";
    }
}

} // namespace winding_order

} // namespace font

inline std::ostream &operator<<(std::ostream &os, enum font::WindingOrder order) {
    return os << font::winding_order::to_string(order);
}

inline std::ostream &operator<<(std::ostream &os, enum font::FontFormat format) {
    return os << font::font_format::to_string(format);
}
