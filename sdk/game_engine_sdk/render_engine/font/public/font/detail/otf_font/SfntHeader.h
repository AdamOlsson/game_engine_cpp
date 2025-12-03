#pragma once

#include "font/detail/ifstream_util.h"
#include "logger/io.h"
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>

namespace font::detail::otf_font {
constexpr uint32_t TTCF = 0x74746366;
constexpr uint32_t OTTO = 0x4F54544F;
constexpr uint32_t TTF = 0x00010000;

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
} // namespace font::detail::otf_font
