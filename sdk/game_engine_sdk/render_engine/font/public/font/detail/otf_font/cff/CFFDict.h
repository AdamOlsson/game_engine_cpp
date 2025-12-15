#pragma once

#include "util/assert.h"
#include <cstdint>
namespace font::detail::otf_font::cff {
struct CFFDict {
    std::vector<int> operators;
    std::vector<std::vector<int>> operands;

    template <std::input_iterator Iter>
    static std::pair<int, std::vector<int>> decode_until_operator(Iter &data,
                                                                  const Iter &end) {
        std::vector<int> operands;
        while (data != end) {
            const int b0 = *data;
            if (32 <= b0 && b0 <= 246) {
                /*std::cout << "decode1: " << std::dec << static_cast<int>(b0) << " ";*/
                operands.push_back(CFFDict::decode1(b0));
                data++;
                continue;
            } else if (247 <= b0 && b0 <= 250) {
                const auto b1 = *(++data);
                /*std::cout << "decode2: " << std::dec << static_cast<int>(b0) << " ";*/
                /*std::cout << std::dec << static_cast<int>(b1) << " ";*/
                operands.push_back(CFFDict::decode2(b0, b1));
                data++;
                continue;
            } else if (251 <= b0 && b0 <= 254) {
                const auto b1 = *(++data);
                /*std::cout << "decode3: " << std::dec << static_cast<int>(b0) << " ";*/
                /*std::cout << std::dec << static_cast<int>(b1) << " ";*/
                operands.push_back(CFFDict::decode3(b0, b1));
                data++;
                continue;
            } else if (b0 == 28) {
                const auto b1 = *(++data);
                const auto b2 = *(++data);
                /*std::cout << "decode3: " << std::dec << static_cast<int>(b0) << " ";*/
                /*std::cout << std::dec << static_cast<int>(b1) << " ";*/
                /*std::cout << std::dec << static_cast<int>(b2) << " ";*/
                operands.push_back(CFFDict::decode4(b1, b2));
                data++;
                continue;
            } else if (b0 == 29) {
                const auto b1 = *(++data);
                const auto b2 = *(++data);
                const auto b3 = *(++data);
                const auto b4 = *(++data);
                /*std::cout << "decode4: " << std::dec << static_cast<int>(b0) << " ";*/
                /*std::cout << std::dec << static_cast<int>(b1) << " ";*/
                /*std::cout << std::dec << static_cast<int>(b2) << " ";*/
                /*std::cout << std::dec << static_cast<int>(b3) << " ";*/
                operands.push_back(CFFDict::decode5(b1, b2, b3, b4));
                data++;
                continue;
            } else if (b0 == 30) {
                DEBUG_ASSERT(false, "Error: operator 30 is not implemented.");
            }
            break;
        }

        int operator_ = *data;
        if (operator_ == 12) {
            int next = *(++data);
            operator_ = (next << 8) | next;
        }

        return std::make_pair(operator_, operands);
    }

    static CFFDict parse(const std::vector<uint8_t> &encoded_bytes) {
        CFFDict dict{};

        const auto iter_end = encoded_bytes.end();
        for (auto iter = encoded_bytes.begin(); iter != iter_end; iter++) {
            auto operator_operands = decode_until_operator(iter, iter_end);
            dict.operators.push_back(operator_operands.first);
            dict.operands.push_back(std::move(operator_operands.second));
        }

        dict.operators.shrink_to_fit();
        dict.operands.shrink_to_fit();
        return dict;
    };

    static int8_t decode1(const uint8_t b0) {
        const auto value = b0 - 139;
        DEBUG_ASSERT(-107 <= value && value <= 107, "Incorrect encoding.");
        return value;
    }

    static int16_t decode2(const uint8_t b0, const uint8_t b1) {
        const auto value = (b0 - 247) * 256 + b1 + 108;
        DEBUG_ASSERT(108 <= value && value <= 1131, "Incorrect encoding.");
        return value;
    }

    static int16_t decode3(const uint8_t b0, const uint8_t b1) {
        const auto value = -(b0 - 251) * 256 - b1 - 108;
        DEBUG_ASSERT(-1131 <= value && value <= -108, "Incorrect encoding.");
        return value;
    }

    static int16_t decode4(const uint8_t b1, const uint8_t b2) {
        const auto value = b1 << 8 | b2;
        DEBUG_ASSERT(-32786 <= value && value <= 32767, "Incorrect encoding.");
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
        throw std::runtime_error(std::format("Unkown operand {}.", static_cast<int>(b0)));
    }

    static float parse_real_number(const std::vector<int> &bytes) {
        DEBUG_ASSERT(bytes[0] == 0x1e, "Error: byte sequence is not a real number.");

        std::vector<uint8_t> nibbles;
        nibbles.reserve(bytes.size() * 2);
        // Note: first byte of real number is always 0x1e and last value is alway 0xf or
        // 0xff
        for (auto i = 1; i < bytes.size(); i++) {
            uint8_t two_nibbles =
                bytes[i]; // No information is lost as these values are originally uint8
            nibbles.push_back((two_nibbles >> 4) & 0xf);
            nibbles.push_back(two_nibbles & 0xf);
        }

        // There are up to 0xf nibbles at the end of the encoding
        if (nibbles.back() == 0xf) {
            nibbles.pop_back();
        }
        if (nibbles.back() == 0xf) {
            nibbles.pop_back();
        }

        /*for (const int &n : nibbles) {*/
        /*    std::cout << "0x" << std::hex << n << std::dec << " ";*/
        /*}*/
        /*std::cout << std::endl;*/

        DEBUG_ASSERT(nibbles.back() != 0xf,
                     "Error: expected to have removed end of number nibble.");

        // Find the nibble index of the decimal point (nibble is 4-bit value)
        int decimal_point_nibble_index = -1;
        for (auto i = 0; i < nibbles.size(); i++) {
            if (nibbles[i] == 0xa) {
                decimal_point_nibble_index = i;
                break;
            }
        }

        DEBUG_ASSERT(decimal_point_nibble_index != -1,
                     "Error: expected real number to contain decimal point.");

        float sign = 1.0f;
        float number = 0.0f;

        // Parse the integer part of the real number
        int magnitude = 1;
        for (int i = decimal_point_nibble_index - 1; i >= 0; i--) {
            uint nibble = nibbles[i];
            if (nibble < 10) {
                number += nibble * magnitude;
                magnitude *= 10;
            } else if (nibble == 0xe) {
                sign = -1.0f;
            } else {
                DEBUG_ASSERT(false, std::format("Error: unexpected nibble {}.", nibble));
            }
        }

        // Parse the decimal part of the real number
        float decimal = 10.0f;
        for (int i = decimal_point_nibble_index + 1; i < nibbles.size(); i++) {
            uint nibble = nibbles[i];
            if (nibble < 10) {
                number += nibble / decimal;
                decimal *= 10.0f;
            } else if (nibble == 0xb) {
                uint8_t next_nibble = nibbles[++i];
                DEBUG_ASSERT(next_nibble < 10,
                             "Error: expected next nibble to be a number.");
                number *= powf(10.0f, next_nibble);
            } else if (nibble == 0xc) {
                uint8_t next_nibble = nibbles[++i];
                DEBUG_ASSERT(next_nibble < 10,
                             "Error: expected next nibble to be a number.");
                number *= powf(10.0f, -next_nibble);
            } else {
                DEBUG_ASSERT(false, std::format("Error: unexpected nibble {}.", nibble));
            }
        }

        return sign * number;
    }

    static std::vector<int> parse_delta(const std::vector<int> &delta) {
        DEBUG_ASSERT(delta.size() > 0,
                     "Error: expected deltas size to be larger than 0.");
        std::vector<int> output;
        output.reserve(delta.size());

        output.push_back(delta[0]);
        for (auto i = 1; i < delta.size(); i++) {
            output.push_back(delta[i] - delta[i - 1]);
        }
        return output;
    }
};
}; // namespace font::detail::otf_font::cff
