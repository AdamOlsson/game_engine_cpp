#pragma once

#include "font/detail/ifstream_util.h"
#include "font/detail/otf_font/cff/CFFStandardStrings.h"
#include "util/assert.h"
#include <cstdint>
#include <memory>
#include <span>
#include <sstream>
#include <vector>

namespace font::detail::otf_font::cff {
struct CFFIndex {
    uint16_t count = 0;
    uint8_t off_size = 0;
    std::unique_ptr<std::vector<uint32_t>> offsets = nullptr;
    std::unique_ptr<std::vector<uint8_t>> data = nullptr;

    static CFFIndex read_index(std::ifstream &stream) {

        const uint16_t count = read_uint16(stream);

        // Empty index consists only of the two bytes in the count
        if (count == 0) {
            return CFFIndex{};
        }

        const uint8_t off_size = read_uint8(stream);

        auto offsets = std::make_unique<std::vector<uint32_t>>();
        offsets->reserve(count + 1);
        for (auto i = 0; i < count + 1; i++) {
            if (off_size == 1) {
                offsets->push_back(read_uint8(stream));
            } else if (off_size == 2) {
                offsets->push_back(read_uint16(stream));
            } else if (off_size == 3) {
                throw std::runtime_error("What the hell, uint24_t?");
            } else if (off_size == 4) {
                offsets->push_back(read_uint32(stream));
            } else {
                throw std::runtime_error(
                    std::format("Unkown offset size value {}.", off_size));
            }
        }

        const auto data_length = offsets->back() - offsets->at(0);
        auto data = read_n_bytes_unique(stream, data_length);

        return CFFIndex{
            .count = count,
            .off_size = off_size,
            .offsets = std::move(offsets),
            .data = std::move(data),
        };
    };

    std::span<uint8_t> operator[](const size_t index) const {
        DEBUG_ASSERT(index < count, std::format("Error: attempted to access beyond "
                                                "CFFIndex count. (index: {}, count: {})",
                                                index, count));
        return std::span(data->data() + offsets->at(index) - offsets->at(0),
                         offsets->at(index + 1) - offsets->at(index));
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "Index {\n"
            << "\tcount: " << count << "\n"
            << "\toff_size: " << static_cast<int>(off_size) << "\n"
            << "\toffsets: " << "[...]" << "\n"
            << "\tdata: " << "[...]" << "\n"
            << "}";
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const CFFIndex &obj) {
        return os << obj.to_string();
    }

    static std::string lookup_string(const CFFIndex &string_index, const int idx) {
        if (idx <= N_STD_STRING) {
            return CFF_STANDARD_STRINGS[idx];
        }
        const std::span<uint8_t> str = string_index[idx - N_STD_STRING];
        return std::string(str.begin(), str.end());
    }
};
}; // namespace font::detail::otf_font::cff
