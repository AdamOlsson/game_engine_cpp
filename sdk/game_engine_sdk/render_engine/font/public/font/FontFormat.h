#pragma once

#include <cstdint>
#include <cstring>
#include <ostream>
namespace font {
enum class FontFormat { TrueType, CFF, Type1, Unknown };
/*enum class WindingOrder { Clockwise, CounterClockwise };*/

enum class FontFill : uint8_t {
    Right = 0,
    Left = 1,
    Unkown = 2,
};

namespace format {

inline const char *to_string(enum FontFormat format) {
    switch (format) {
    case FontFormat::TrueType:
        return "TrueType";
    case FontFormat::CFF:
        return "CFF";
    case FontFormat::Type1:
        return "Type 1";
    default:
        return "Unknown";
    }
}

inline enum FontFormat from_string(const char *format) {
    if (format == nullptr) {
        return FontFormat::Unknown;
    }

    if (strcmp(format, "TrueType") == 0) {
        return FontFormat::TrueType;
    } else if (strcmp(format, "CFF") == 0) {
        return FontFormat::CFF;
    } else if (strcmp(format, "Type 1") == 0) {
        return FontFormat::Type1;
    }

    return FontFormat::Unknown; // Default fallback
}

} // namespace format

namespace fill {

inline const char *to_string(enum FontFill fill) {
    switch (fill) {
    case FontFill::Left:
        return "Left";
    case FontFill::Right:
        return "Right";
    case FontFill::Unkown:
        return "Unknown";
    }
}

} // namespace fill

inline constexpr FontFill operator!(const FontFill fill) {
    switch (fill) {
    case FontFill::Left:
        return FontFill::Right;
    case FontFill::Right:
        return FontFill::Left;
    case FontFill::Unkown:
        return FontFill::Unkown;
    }
}

/*namespace winding_order {*/
/**/
/*inline WindingOrder from_font_format(const FontFormat format) {*/
/*    switch (format) {*/
/*    case FontFormat::TrueType:*/
/*        return WindingOrder::Clockwise;*/
/*    case FontFormat::CFF:*/
/*    case TrueType::Type1:*/
/*        return WindingOrder::CounterClockwise;*/
/*    default:*/
/*        return WindingOrder::Clockwise; // Default fallback*/
/*    }*/
/*}*/
/**/
/*inline const char *to_string(enum WindingOrder order) {*/
/*    switch (order) {*/
/*    case WindingOrder::Clockwise:*/
/*        return "Clockwise";*/
/*    case WindingOrder::CounterClockwise:*/
/*        return "CounterClockwise";*/
/*    default:*/
/*        return "Unknown";*/
/*    }*/
/*}*/
/**/
/*} // namespace winding_order*/

} // namespace font

/*inline std::ostream &operator<<(std::ostream &os, enum font::WindingOrder order) {*/
/*    return os << font::winding_order::to_string(order);*/
/*}*/

inline std::ostream &operator<<(std::ostream &os, enum font::FontFormat format) {
    return os << font::format::to_string(format);
}

inline std::ostream &operator<<(std::ostream &os, enum font::FontFill fill) {
    return os << font::fill::to_string(fill);
}
