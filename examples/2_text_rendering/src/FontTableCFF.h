#pragma once

#include "CFFStandardStrings.h"
#include "ifstream_util.h"
#include "logger/io.h"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <span>
#include <sstream>

using Card8 = uint8_t;
using Card16 = uint16_t;

using OffSize = uint8_t;
using Offset1 = uint8_t;
using Offset2 = uint16_t;
// using Offset3 = uint24_t; // Hope I never get to use this
using Offset4 = uint32_t;

namespace cff {

struct Index {
    Card16 count = 0;
    OffSize off_size = 0;
    std::unique_ptr<std::vector<uint32_t>> offsets = nullptr;
    std::unique_ptr<std::vector<uint8_t>> data = nullptr;

    static Index read_index(std::ifstream &stream) {

        const auto count = read_uint16(stream);
        const auto off_size = read_uint8(stream);

        auto offsets = std::make_unique<std::vector<uint32_t>>();
        offsets->reserve(count + 1);
        for (auto i = 0; i < count + 1; i++) {
            if (off_size == 1) {
                offsets->push_back(read_uint8(stream));
            } else if (off_size == 2) {
                offsets->push_back(read_uint16(stream));
            } else if (off_size == 3) {
                std::runtime_error("What the hell, uint24_t?");
            } else if (off_size == 4) {
                offsets->push_back(read_uint32(stream));
            } else {
                std::runtime_error("Unkown offset size value");
            }
        }

        const auto data_length = offsets->back() - offsets->at(0);
        auto data = read_n_bytes_unique(stream, data_length);

        return Index{
            .count = count,
            .off_size = off_size,
            .offsets = std::move(offsets),
            .data = std::move(data),
        };
    };

    std::span<uint8_t> operator[](const size_t index) const {
        return std::span(data->data() + offsets->at(index) - offsets->at(0),
                         offsets->at(index + 1) - offsets->at(index));
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "Index{\n"
            << "\tcount: " << count << "\n"
            << "\toff_size: " << static_cast<int>(off_size) << "\n"
            << "\toffsets: " << "[...]" << "\n"
            << "\tdata: " << "[...]" << "\n"
            << "}";
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const Index &obj) {
        return os << obj.to_string();
    }
};

struct Dict {
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

    static uint16_t decode3(const uint8_t b0, const uint8_t b1) {
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
};

enum TopDictValue {
    Version = 0,
    Notice = 1,
    FullName = 2,
    FamilyName = 3,
    Weight = 4,

    Copyright = 9999,
    // TODO: More here
};

struct FontTableCFF {
    struct Header {
        Card8 major_version = 0;
        Card8 minor_version = 0;
        Card8 hdr_size = 0;
        OffSize off_size = 0;
    } header;

    Index name;

    struct Top {
        std::string version = "";
        std::string notice = "";
        std::string copyright = "";
        std::string full_name = "";
        std::string family_name = "";
        std::string weight = "";
    } top;

    static FontTableCFF read_font_table_cff(std::ifstream &stream) {
        auto cff = FontTableCFF{
            .header =
                {
                    .major_version = read_uint8(stream),
                    .minor_version = read_uint8(stream),
                    .hdr_size = read_uint8(stream),
                    .off_size = read_uint8(stream),
                },
            .name = Index::read_index(stream),
        };

        /*for (const auto &c : cff.name[0]) {*/
        /*    std::cout << c;*/
        /*}*/
        /*std::cout << std::endl;*/

        const auto top_index = Index::read_index(stream);
        const auto string_index = Index::read_index(stream);
        cff.top = parse_top_data(top_index, string_index);

        std::cout << "top_index: " << top_index << std::endl;
        /*std::cout << "top_index[0] (hex): ";*/
        /*for (const auto &c : top_index[0]) {*/
        /*    std::cout << std::hex << static_cast<int>(c) << " ";*/
        /*}*/
        std::cout << std::endl;

        std::cout << "top_index[0] (dec): ";
        for (const auto &c : top_index[0]) {
            std::cout << std::dec << static_cast<int>(c) << " ";
        }
        std::cout << std::endl;

        /*std::cout << "string_index: " << string_index << std::endl;*/
        /*for (const auto &c : string_index[1]) {*/
        /*    std::cout << c << " ";*/
        /*}*/
        /*std::cout << std::endl;*/

        return cff;
    }

    static Top parse_top_data(const auto &top_index, const auto &string_index) {
        auto top = Top{};
        const auto data = top_index[0];
        bool break_loop = false; // Temporary
        for (auto i = 0; i < data.size(); i++) {
            int value = 0;
            const auto b0 = data[i];
            /*std::cout << "b0: " << std::dec << static_cast<int>(b0) << std::endl;*/
            std::cout << std::dec << static_cast<int>(b0) << " ";

            // TODO: What does 2 byte operand mean?
            if (32 <= b0 && b0 <= 246) {
                value = Dict::decode1(b0);
            } else if (247 <= b0 && b0 <= 250) {
                const auto b1 = data[++i];

                std::cout << std::dec << static_cast<int>(b1) << " ";
                value = Dict::decode2(b0, b1);
            } else if (251 <= b0 && b0 <= 254) {
                const auto b1 = data[++i];
                value = Dict::decode3(b0, b1);
            } else if (b0 == 28) {
                const auto b1 = data[++i];
                const auto b2 = data[++i];
                value = Dict::decode4(b1, b2);
            } else if (b0 == 29) {
                const auto b1 = data[++i];
                const auto b2 = data[++i];
                const auto b3 = data[++i];
                const auto b4 = data[++i];
                value = Dict::decode5(b1, b2, b3, b4);
            } else {
                throw std::runtime_error(std::format("Unkown operand {}", b0));
            }

            // Note: "operator" is reserved keyword, therefore the underscore
            const int operator_ = data[++i];

            /*std::cout << "operator: " << std::dec << static_cast<int>(operator_)*/
            /*          << std::endl;*/
            std::cout << std::dec << static_cast<int>(operator_) << std::endl;

            switch (operator_) {
            case TopDictValue::Version: {
                top.version = lookup_string(string_index, value);
                break;
            }
            case TopDictValue::Notice: {
                top.notice = lookup_string(string_index, value);
                break;
            }
            case TopDictValue::FullName: {
                top.full_name = lookup_string(string_index, value);
                break;
            }
            case TopDictValue::FamilyName: {
                top.family_name = lookup_string(string_index, value);
                break;
            }
            case TopDictValue::Weight: {
                top.weight = lookup_string(string_index, value);
                break;
            }
            case TopDictValue::Copyright: {
                break_loop = true;
                break;
            }
            default:
                break_loop = true;
                break;
            }

            if (break_loop) {
                break;
            }
        }

        return top;
    }

    static std::string lookup_string(const Index &string_index, const int idx) {
        if (idx <= N_STD_STRING) {
            return CFF_STANDARD_STRINGS[idx];
        }
        const std::span<uint8_t> str = string_index[idx - N_STD_STRING];
        return std::string(str.begin(), str.end());
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "FontTableCFF {\n"
            << "\theader: {" << "\n"
            << "\t\tmajor_version: " << static_cast<int>(header.major_version) << "\n"
            << "\t\tminor_version: " << static_cast<int>(header.minor_version) << "\n"
            << "\t\thdr_size: " << static_cast<int>(header.hdr_size) << "\n"
            << "\t\toff_size: " << static_cast<int>(header.off_size) << "\n"
            << "\t}" << "\n"
            << "\ttop: {" << "\n"
            << "\t\tversion: " << top.version << "\n"
            << "\t\tnotice: " << top.notice << "\n"
            << "\t\tcopyright: " << top.copyright << "\n"
            << "\t\tfull_name: " << top.full_name << "\n"
            << "\t\tfamily_name: " << top.family_name << "\n"
            << "\t\tweight: " << top.weight << "\n"
            << "\t}" << "\n"
            << "}";
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const FontTableCFF &obj) {
        return os << obj.to_string();
    }
};

} // namespace cff
