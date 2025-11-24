
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

uint16_t read_uint16(std::ifstream &stream) {
    uint16_t value = 0;
    value |= stream.get() << 8;
    value |= stream.get();
    return value;
}

struct SFntHeader {
    uint32_t sfnt_version = 0;
    uint16_t num_tables = 0;
    uint16_t search_range = 0;
    uint16_t entry_selector = 0;
    uint16_t range_shift = 0;

    std::string to_string() const {
        std::ostringstream oss;
        oss << "SFntHeader {\n"
            << "\tsfnt_version: 0x" << std::hex << std::uppercase << sfnt_version
            << std::dec << "\n"
            << "\tnum_tables: " << num_tables << "\n"
            << "\tsearch_range: " << search_range << "\n"
            << "\tentry_selector: " << entry_selector << "\n"
            << "\trange_shift: " << range_shift << "\n"
            << "}";
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const SFntHeader &obj) {
        return os << obj.to_string();
    }
};

SFntHeader read_sfnt_header(std::ifstream &stream) {
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

    return header;
}

int main() {
    /*auto otf_filestream = open_filestream(ASSET_FILE("ftystrategycidencv.otf"));*/
    /*auto otf_filestream = open_filestream(ASSET_FILE("dustismo-roman-italic.ttf"));*/
    auto otf_filestream =
        open_filestream(ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf"));

    const auto sfnt_header = read_sfnt_header(otf_filestream);
    std::cout << sfnt_header << std::endl;

    return 0;
}
