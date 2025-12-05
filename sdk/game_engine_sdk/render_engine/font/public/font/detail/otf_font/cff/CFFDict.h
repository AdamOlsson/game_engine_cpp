#pragma once

#include "util/assert.h"
#include <cstdint>
namespace font::detail::otf_font::cff {
struct CFFDict {
    static uint8_t decode1(const uint8_t b0) {
        const auto value = b0 - 139;
        DEBUG_ASSERT(-107 <= value && value <= 107, "Incorrect encoding");
        return value;
    }

    static uint16_t decode2(const uint8_t b0, const uint8_t b1) {
        const auto value = (b0 - 247) * 256 + b1 + 108;
        DEBUG_ASSERT(108 <= value && value <= 1131, "Incorrect encoding");
        return value;
    }

    static int16_t decode3(const uint8_t b0, const uint8_t b1) {
        const auto value = -(b0 - 251) * 256 - b1 - 108;
        DEBUG_ASSERT(-1131 <= value && value <= -108, "Incorrect encoding");
        return value;
    }

    static uint16_t decode4(const uint8_t b1, const uint8_t b2) {
        const auto value = b1 << 8 | b2;
        DEBUG_ASSERT(-32786 <= value && value <= 32767, "Incorrect encoding");
        return value;
    }

    static uint64_t decode5(const uint8_t b1, const uint8_t b2, const uint8_t b3,
                            const uint8_t b4) {
        return b1 << 24 | b2 << 16 | b3 << 8 | b4;
    }

    template <std::input_iterator Iter> static int decode_one_value(Iter &data) {
        const auto b0 = *data;
        if (32 <= b0 && b0 <= 246) {
            /*std::cout << "decode1: " << std::dec << static_cast<int>(b0) << " ";*/
            return CFFDict::decode1(b0);
        } else if (247 <= b0 && b0 <= 250) {
            const auto b1 = *(++data);
            /*std::cout << "decode2: " << std::dec << static_cast<int>(b0) << " ";*/
            /*std::cout << std::dec << static_cast<int>(b1) << " ";*/
            return CFFDict::decode2(b0, b1);
        } else if (251 <= b0 && b0 <= 254) {
            const auto b1 = *(++data);
            /*std::cout << "decode3: " << std::dec << static_cast<int>(b0) << " ";*/
            /*std::cout << std::dec << static_cast<int>(b1) << " ";*/
            return CFFDict::decode3(b0, b1);
        } else if (b0 == 28) {
            const auto b1 = *(++data);
            const auto b2 = *(++data);
            /*std::cout << "decode3: " << std::dec << static_cast<int>(b0) << " ";*/
            /*std::cout << std::dec << static_cast<int>(b1) << " ";*/
            /*std::cout << std::dec << static_cast<int>(b2) << " ";*/
            return CFFDict::decode4(b1, b2);
        } else if (b0 == 29) {
            const auto b1 = *(++data);
            const auto b2 = *(++data);
            const auto b3 = *(++data);
            const auto b4 = *(++data);
            /*std::cout << "decode4: " << std::dec << static_cast<int>(b0) << " ";*/
            /*std::cout << std::dec << static_cast<int>(b1) << " ";*/
            /*std::cout << std::dec << static_cast<int>(b2) << " ";*/
            /*std::cout << std::dec << static_cast<int>(b3) << " ";*/
            return CFFDict::decode5(b1, b2, b3, b4);
        }
        throw std::runtime_error(std::format("Unkown operand {}", static_cast<int>(b0)));
    }
};
}; // namespace font::detail::otf_font::cff
