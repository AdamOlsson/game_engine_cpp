
#include "tiling/traits.h"
#include <fstream>
#include <iostream>
#include <sstream>

#define ASSET_FILE(filename) ASSET_DIR "/" filename

std::vector<char> read_file(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error(std::format("Failed to open file {}", filename));
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}

std::ifstream open_filestream(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error(std::format("Failed to open file {}", filename));
    }

    file.seekg(0);
    return file;
}

// OTF Data types:
// https://learn.microsoft.com/en-us/typography/opentype/spec/otff#data-types
constexpr uint32_t TTCF = 0x74746366;
constexpr uint32_t OTTO = 0x4F54544F;
constexpr uint32_t TTF = 0x00010000;

uint32_t read_uint32(std::ifstream &stream) {
    uint32_t value = 0;
    value |= stream.get() << 24;
    value |= stream.get() << 16;
    value |= stream.get() << 8;
    value |= stream.get();
    return value;
}

int64_t read_int64(std::ifstream &stream) {
    int64_t value = 0;
    value |= read_uint32(stream);
    value = value << 32;
    value |= read_uint32(stream);
    return value;
}

uint64_t read_uint64(std::ifstream &stream) {
    uint64_t value = 0;
    value |= read_uint32(stream);
    value = value << 32;
    value |= read_uint32(stream);
    return value;
}

int16_t read_int16(std::ifstream &stream) {
    int16_t value = 0;
    value |= stream.get() << 8;
    value |= stream.get();
    return value;
}

uint16_t read_uint16(std::ifstream &stream) {
    uint16_t value = 0;
    value |= stream.get() << 8;
    value |= stream.get();
    return value;
}

uint8_t read_uint8(std::ifstream &stream) {
    uint8_t value = 0;
    value |= stream.get();
    return value;
}

std::vector<char> read_n_bytes(std::ifstream &stream, const size_t n) {
    std::vector<char> buffer(n);
    stream.read(buffer.data(), n);
    return buffer;
}

struct TableRecord {
    std::string table_tag = 0;
    uint32_t checksum = 0;
    uint32_t offset = 0;
    uint32_t length = 0;

    static TableRecord read_table_record(std::ifstream &stream) {
        const uint32_t tag_bytes = read_uint32(stream);
        auto table_tag = std::format("{:c}{:c}{:c}{:c}", (tag_bytes >> 24) & 0xFF,
                                     (tag_bytes >> 16) & 0xFF, (tag_bytes >> 8) & 0xFF,
                                     tag_bytes & 0xFF);

        return TableRecord{
            .table_tag = std::move(table_tag),
            .checksum = read_uint32(stream),
            .offset = read_uint32(stream),
            .length = read_uint32(stream),
        };
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "TableRecord {\n"
            << "\ttable_tag: " << table_tag << "\n"
            << "\tchecksum: 0x" << checksum << std::dec << "\n"
            << "\toffset: " << offset << "\n"
            << "\tlength: " << length << "\n"
            << "}";
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const TableRecord &obj) {
        return os << obj.to_string();
    }
};

struct SFntHeader {
    uint32_t sfnt_version = 0;
    uint16_t num_tables = 0;
    uint16_t search_range = 0;
    uint16_t entry_selector = 0;
    uint16_t range_shift = 0;
    std::unique_ptr<std::vector<TableRecord>> table_records;

    std::string to_string() const {
        std::ostringstream oss;
        oss << "SFntHeader {\n"
            << "\tsfnt_version: 0x" << std::hex << std::uppercase << sfnt_version
            << std::dec << "\n"
            << "\tnum_tables: " << num_tables << "\n"
            << "\tsearch_range: " << search_range << "\n"
            << "\tentry_selector: " << entry_selector << "\n"
            << "\trange_shift: " << range_shift << "\n"
            << "\ttable_records: " << "[...]" << "\n"
            << "}";
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const SFntHeader &obj) {
        return os << obj.to_string();
    }

    static SFntHeader read_sfnt_header(std::ifstream &stream) {
        uint32_t tag = read_uint32(stream);

        if (tag == TTCF) {
            throw std::runtime_error("OpenType Collection files are not supported.");
        }

        if (tag == TTF) {
            throw std::runtime_error("OpenType outline types are not supported.");
        }

        auto header = SFntHeader{
            .sfnt_version = tag,
            .num_tables = read_uint16(stream),
            .search_range = read_uint16(stream),
            .entry_selector = read_uint16(stream),
            .range_shift = read_uint16(stream),
            .table_records = std::make_unique<std::vector<TableRecord>>(),
        };

        // Validate entrySelector: should be floor(log2(numTables))
        DEBUG_ASSERT(header.entry_selector ==
                         static_cast<uint16_t>(std::floor(std::log2(header.num_tables))),
                     "entrySelector should equal floor(log2(numTables))");

        // Validate searchRange: should be 16 * 2^entrySelector
        DEBUG_ASSERT(header.search_range == 16 * (1 << header.entry_selector),
                     "searchRange should equal 16 * 2^entrySelector");

        // Validate rangeShift: should be numTables*16 - searchRange
        DEBUG_ASSERT(header.range_shift == (header.num_tables * 16) - header.search_range,
                     "rangeShift should equal numTables*16 - searchRange");

        header.table_records->reserve(header.num_tables);
        for (auto i = 0; i < header.num_tables; i++) {
            header.table_records->push_back(TableRecord::read_table_record(stream));
        }
        // Sort the tables such that I can read them sequentially
        std::sort(header.table_records->begin(), header.table_records->end(),
                  [](const auto &a, const auto &b) { return a.offset < b.offset; });

        return header;
    }
};

struct FontTableHead {
    uint16_t major_version = 0;
    uint16_t minor_version = 0;
    uint32_t font_revision = 0;
    uint32_t checksum_adjustment = 0;
    uint32_t magic_number = 0;
    uint16_t flags = 0;
    uint16_t units_per_em = 0;
    int64_t created = 0;
    int64_t modified = 0;
    int16_t x_min = 0;
    int16_t y_min = 0;
    int16_t x_max = 0;
    int16_t y_max = 0;
    uint16_t mac_style = 0;
    uint16_t lowest_rec_ppem = 0;
    int16_t font_direction_hint = 0;
    int16_t index_to_loc_format = 0;
    int16_t glyph_data_format = 0;

    static FontTableHead read_font_table_head(std::ifstream &stream) {
        return FontTableHead{
            .major_version = read_uint16(stream),
            .minor_version = read_uint16(stream),
            .font_revision = read_uint32(stream),
            .checksum_adjustment = read_uint32(stream),
            .magic_number = read_uint32(stream),
            .flags = read_uint16(stream),
            .units_per_em = read_uint16(stream),
            .created = read_int64(stream),
            .modified = read_int64(stream),
            .x_min = read_int16(stream),
            .y_min = read_int16(stream),
            .x_max = read_int16(stream),
            .y_max = read_int16(stream),
            .mac_style = read_uint16(stream),
            .lowest_rec_ppem = read_uint16(stream),
            .font_direction_hint = read_int16(stream),
            .index_to_loc_format = read_int16(stream),
            .glyph_data_format = read_int16(stream),
        };
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "FontTableHead {\n"
            << "\tmajor_version: " << major_version << "\n"
            << "\tminor_version: " << minor_version << "\n"
            << "\tfont_revision: 0x" << std::hex << font_revision << std::dec << "\n"
            << "\tchecksum_adjustment: 0x" << std::hex << checksum_adjustment << std::dec
            << "\n"
            << "\tmagic_number: 0x" << std::hex << magic_number << std::dec << "\n"
            << "\tflags: 0x" << std::hex << flags << std::dec << "\n"
            << "\tunits_per_em: " << units_per_em << "\n"
            << "\tcreated: " << created << "\n"
            << "\tmodified: " << modified << "\n"
            << "\tx_min: " << x_min << "\n"
            << "\ty_min: " << y_min << "\n"
            << "\tx_max: " << x_max << "\n"
            << "\ty_max: " << y_max << "\n"
            << "\tmac_style: 0x" << std::hex << mac_style << std::dec << "\n"
            << "\tlowest_rec_ppem: " << lowest_rec_ppem << "\n"
            << "\tfont_direction_hint: " << font_direction_hint << "\n"
            << "\tindex_to_loc_format: " << index_to_loc_format << "\n"
            << "\tglyph_data_format: " << glyph_data_format << "\n"
            << "}";
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const FontTableHead &obj) {
        return os << obj.to_string();
    }
};

struct FontTableEncodingRecord {};
struct FontTableCmap {
    uint16_t version = 0;
    uint16_t num_tables = 0;
};

int main() {
    /*auto otf_filestream = open_filestream(ASSET_FILE("ftystrategycidencv.otf"));*/
    /*auto otf_filestream = open_filestream(ASSET_FILE("dustismo-roman-italic.ttf"));*/
    auto otf_filestream =
        open_filestream(ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf"));

    size_t bytes_read = 0;
    const auto sfnt_header = SFntHeader::read_sfnt_header(otf_filestream);
    bytes_read += sizeof(sfnt_header);
    std::cout << sfnt_header << std::endl;

    std::cout << "Found tables:" << std::endl;
    for (const auto &t : *sfnt_header.table_records) {
        std::cout << " - " << t.table_tag << std::endl;
    }

    const auto font_table_head = FontTableHead::read_font_table_head(otf_filestream);
    bytes_read += sizeof(font_table_head);

    // TODO: Calculate checksum of TableRecord
    std::cout << font_table_head << std::endl;

    // TODO: Continue with reading Cmap and CCF headers

    return 0;
}
