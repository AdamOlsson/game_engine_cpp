#pragma once

#include "ifstream_util.h"
#include <cstdint>
#include <sstream>

namespace cmap {
struct FontTableEncodingRecord {
    uint16_t platform_id = 0;
    uint16_t encoding_id = 0;
    uint32_t subtable_offset = 0;

    static FontTableEncodingRecord read_encoding_record(std::ifstream &stream) {
        return FontTableEncodingRecord{
            .platform_id = read_uint16(stream),
            .encoding_id = read_uint16(stream),
            .subtable_offset = read_uint32(stream),
        };
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "FontTableEncodingRecord {\n"
            << "\tplatform_id: " << platform_id << "\n"
            << "\tencoding_id: " << encoding_id << "\n"
            << "\tsubtable_offset: " << subtable_offset << "\n"
            << "}";
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os,
                                    const FontTableEncodingRecord &obj) {
        return os << obj.to_string();
    }
};

struct FontTableCmap {
    uint16_t version = 0;
    uint16_t num_tables = 0;
    std::unique_ptr<std::vector<FontTableEncodingRecord>> encoding_records;

    static FontTableCmap read_font_table_cmap(std::ifstream &stream) {

        auto cmap = FontTableCmap{
            .version = read_uint16(stream),
            .num_tables = read_uint16(stream),
            .encoding_records = std::make_unique<std::vector<FontTableEncodingRecord>>(),
        };

        cmap.encoding_records->reserve(cmap.num_tables);
        for (auto i = 0; i < cmap.num_tables; ++i) {
            cmap.encoding_records->push_back(
                FontTableEncodingRecord::read_encoding_record(stream));
        }

        // TODO: There is more to do here
        // https://learn.microsoft.com/en-us/typography/opentype/spec/cmap

        return cmap;
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "FontTableCmap {\n"
            << "\tversion: " << version << "\n"
            << "\tnum_tables: " << num_tables << "\n"
            << "\tencoding_records: [...]\n"
            << "}";
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const FontTableCmap &obj) {
        return os << obj.to_string();
    }
};

struct Subtable {};

} // namespace cmap
