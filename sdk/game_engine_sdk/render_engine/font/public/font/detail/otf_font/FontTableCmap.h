#pragma once

#include "font/detail/ifstream_util.h"
#include "util/assert.h"
#include <cstdint>
#include <sstream>

namespace font::detail::otf_font::cmap {

enum class PlatformId : uint16_t {
    Unicode = 0,
    Macintosh = 1,
    ISO = 2,
    Windows = 3,
    Custom = 4,
};

inline std::ostream &operator<<(std::ostream &os, PlatformId type) {
    switch (type) {
    case PlatformId::Unicode:
        return os << "PlatformId::Unicode";
    case PlatformId::Macintosh:
        return os << "PlatformId::Macintosh";
    case PlatformId::ISO:
        return os << "PlatformId::ISO";
    case PlatformId::Windows:
        return os << "PlatformId::Windows";
    case PlatformId::Custom:
        return os << "PlatformId::Custom";
    }
    DEBUG_ASSERT(false,
                 std::format("Error: Invalid platform id {}", static_cast<int>(type)));
}

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
            << "\tplatform_id: " << platform_id << " ("
            << static_cast<PlatformId>(platform_id) << ")"
            << "\n"
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

struct SubtableFormat0 {
    uint16_t format = 0;
    uint16_t length = 0;
    uint16_t language = 0;
    std::array<uint8_t, 256> glyph_id_array{0};

    uint8_t get_glyph_id(const uint8_t char_code) const {
        return glyph_id_array[char_code];
    }

    static SubtableFormat0 read_subtable(std::ifstream &stream) {
        // format read in the parent function          // 2 bytes
        const uint16_t length = read_uint16(stream);   // 4 bytes
        const uint16_t language = read_uint16(stream); // 6 bytes
        const unsigned int bytes_read = 6;

        std::array<uint8_t, 256> glyph_id_array{0};
        const unsigned int bytes_remaining = length - bytes_read;
        DEBUG_ASSERT(bytes_remaining <= 256,
                     std::format("Error: Number of bytes remaining ({}) goes beyond "
                                 "capacity of array (256).",
                                 bytes_remaining));

        for (auto i = 0; i < bytes_remaining; i++) {
            glyph_id_array[i] = read_uint8(stream);
        }

        return SubtableFormat0{
            .length = length,
            .language = language,
            .glyph_id_array = std::move(glyph_id_array),
        };
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "SubtableFormat0 {\n"
            << "\tformat: " << format << "\n"
            << "\tlength: " << length << "\n"
            << "\tlanguage: " << language << "\n"
            << "\tglyph_id_array: " << "[...]" << "\n"
            << "}";
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const SubtableFormat0 &obj) {
        return os << obj.to_string();
    }
};

struct SubtableFormat4 {
    uint16_t format = 4;
    uint16_t length = 0;
    uint16_t language = 0;
    uint16_t seg_count_x2 = 0;
    uint16_t search_range = 0;
    uint16_t entry_selector = 0;
    uint16_t range_shift = 0;
    std::vector<uint16_t> end_code;
    uint16_t reserved = 0;
    std::vector<uint16_t> start_code;
    std::vector<int16_t> id_delta;
    std::vector<uint16_t> id_range_offset;
    std::vector<uint16_t> glyph_id_array;

    uint16_t get_glyph_id(const uint16_t char_code) const {
        // TODO: Binary search, use seg_count, search range, entry selector and range
        // shift for this. Also note the final start and end code must be 0xFFFF, this can
        // be used in ending the binary search
        int index = -1;
        for (auto i = 0; i < end_code.size(); i++) {
            if (char_code <= end_code[i]) {
                index = i;
                break;
            }
        }

        if (index == -1) {
            return 0; // missing glyph
        }

        if (start_code[index] > char_code) {
            return 0; // missing glyph
        }

        if (id_range_offset[index] == 0) {
            return (char_code + id_delta[index]) % 65536;
        }

        const int offset = id_range_offset[index] / 2;
        int array_index = offset + (char_code - start_code[index]);
        array_index -= ((seg_count_x2 / 2) - index);

        if (array_index >= 0 && array_index < glyph_id_array.size()) {
            int glyph_id = glyph_id_array[array_index];

            if (glyph_id != 0) {
                return (glyph_id + id_delta[index]) % 65536;
            }
        }

        return 0; // Missing glyph
    }

    static SubtableFormat4 read_subtable(std::ifstream &stream) {
        // format read in the parent function
        // const uint16_t format = read_uint16(stream);         // 2 bytes
        const uint16_t length = read_uint16(stream);         // 4 bytes
        const uint16_t language = read_uint16(stream);       // 6 bytes
        const uint16_t seg_count_x2 = read_uint16(stream);   // 8 bytes
        const uint16_t search_range = read_uint16(stream);   // 10 bytes
        const uint16_t entry_selector = read_uint16(stream); // 12 bytes
        const uint16_t range_shift = read_uint16(stream);    // 14 bytes
        int bytes_read = 14;

        const int seg_count = seg_count_x2 / 2;
        std::vector<uint16_t> end_code = read_n_uint16(stream, seg_count);
        bytes_read += seg_count_x2;

        read_uint16(stream); // reserved
        bytes_read += 2;     // 2 bytes for the reserved field

        std::vector<uint16_t> start_code = read_n_uint16(stream, seg_count);
        bytes_read += seg_count_x2;

        std::vector<int16_t> id_delta = read_n_int16(stream, seg_count);
        bytes_read += seg_count_x2;

        std::vector<uint16_t> id_range_offset = read_n_uint16(stream, seg_count);
        bytes_read += seg_count_x2;

        DEBUG_ASSERT(length - bytes_read >= 0,
                     "Error: Can't read negative number of bytes for glyph id array");
        std::vector<uint16_t> glyph_id_array = read_n_uint16(stream, length - bytes_read);

        return SubtableFormat4{
            .length = length,
            .language = language,
            .seg_count_x2 = seg_count_x2,
            .search_range = search_range,
            .entry_selector = entry_selector,
            .range_shift = range_shift,
            .end_code = std::move(end_code),
            .start_code = std::move(start_code),
            .id_delta = std::move(id_delta),
            .id_range_offset = std::move(id_range_offset),
            .glyph_id_array = std::move(glyph_id_array),
        };
    }

    std::string to_string() const {
        std::ostringstream oss;

        oss << "SubtableFormat4 {\n"
            << "\tformat: " << format << "\n"
            << "\tlength: " << length << "\n"
            << "\tlanguage: " << language << "\n"
            << "\tseg_count_x2: " << seg_count_x2 << "\n"
            << "\tsearch_range: " << search_range << "\n"
            << "\tentry_selector: " << entry_selector << "\n"
            << "\trange_shift: " << range_shift << "\n"
            << "\tend_code: " << "[...]" << "\n"
            << "\tstart_code: " << "[...]" << "\n"
            << "\tid_delta: " << "[...]" << "\n"
            << "\tid_range_offset: " << "[...]" << "\n"
            << "\tglyph_id_array: " << "[...]" << "\n"
            << "}";

        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const SubtableFormat4 &obj) {
        return os << obj.to_string();
    }
};

struct FontTableCmap {
    uint16_t version = 0;
    uint16_t num_tables = 0;

    std::optional<size_t> preferred_table_idx;
    struct Subtable {
        FontTableEncodingRecord record;
        std::variant<SubtableFormat0, SubtableFormat4> table;
    };
    std::vector<Subtable> tables;

    uint16_t get_glyph_id(uint16_t char_code) const {
        DEBUG_ASSERT(preferred_table_idx.has_value(),
                     "Error: No preferred table index was found");

        const auto &table = tables[preferred_table_idx.value()];

        return std::visit(
            [char_code](const auto &subtable) -> uint16_t {
                return subtable.get_glyph_id(char_code);
            },
            table.table);
    }

    static FontTableCmap read_font_table_cmap(std::ifstream &stream) {

        const auto start_cmap_data = stream.tellg();

        const auto version = read_uint16(stream);
        const auto num_tables = read_uint16(stream);
        std::vector<Subtable> tables;
        tables.reserve(num_tables);

        std::vector<FontTableEncodingRecord> encoding_records;
        encoding_records.reserve(num_tables);
        for (auto i = 0; i < num_tables; ++i) {
            encoding_records.push_back(
                FontTableEncodingRecord::read_encoding_record(stream));
        }

        for (auto &record : encoding_records) {
            stream.seekg(start_cmap_data + std::streamoff(record.subtable_offset));
            DEBUG_ASSERT(
                stream.good(),
                "Error: Filestream not okay after seeking to cmap subtable data.");

            const uint16_t format = read_uint16(stream);
            if (format == 0) {
                tables.push_back({.record = std::move(record),
                                  .table = SubtableFormat0::read_subtable(stream)});
            } else if (format == 4) {
                tables.push_back({.record = std::move(record),
                                  .table = SubtableFormat4::read_subtable(stream)});
            } else {
                throw std::runtime_error(std::format(
                    "Error: cmap subtable format {} not yet implemented", format));
            }
        }

        return FontTableCmap{
            .version = version,
            .num_tables = num_tables,
            .preferred_table_idx = find_preferred_table(tables),
            .tables = std::move(tables),
        };
        ;
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

  private:
    static std::optional<size_t>
    find_preferred_table(const std::vector<Subtable> &tables) {
        for (size_t i = 0; i < tables.size(); i++) {
            const auto &rec = tables[i].record;
            if (rec.platform_id == static_cast<uint16_t>(PlatformId::Unicode)) {
                return i;
            }
        }
        return std::nullopt;
    }
};

} // namespace font::detail::otf_font::cmap
