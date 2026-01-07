#include "font/OTFFont.h"

#include "font/detail/ifstream_util.h"
#include "font/detail/otf_font/FontTableHead.h"
#include "font/detail/otf_font/FontTableHhea.h"
#include "font/detail/otf_font/SfntHeader.h"
#include "font/detail/otf_font/cff/FontTableCFF.h"

font::OTFFont::OTFFont(const std::string &filepath) {

    std::ifstream otf_filestream = font::detail::open_filestream(filepath);
    auto m_sfnt_header =
        font::detail::otf_font::SFntHeader::read_sfnt_header(otf_filestream);
    /*std::cout << m_sfnt_header << std::endl;*/

    /*std::cout << "Found tables:" << std::endl;*/
    for (const auto &t : *m_sfnt_header.table_records) {

        if (!otf_filestream) {
            throw std::runtime_error(
                std::format("Stream error after read: ", otf_filestream.rdstate()));
        }

        otf_filestream.seekg(t.offset);
        if (!validate_table(otf_filestream, t)) {
            throw std::runtime_error(
                std::format("Error: Table {} checksums does not match.", t.table_tag));
        }

        /*std::cout << "Seeking to " << t.offset << " (0x" << std::hex << t.offset <<
         * ")"*/
        /*          << std::endl;*/
        /*std::cout << " # " << t.table_tag << std::endl;*/

        if (t.table_tag == "head") {
            // TODO: Calculate checksum of TableRecord
            auto m_font_table_head =
                font::detail::otf_font::FontTableHead::read_font_table_head(
                    otf_filestream);
            /*std::cout << " - " << m_font_table_head << std::endl;*/

        } else if (t.table_tag == "hhea") {
            auto m_font_table_hhea =
                font::detail::otf_font::FontTableHhea::read_font_table_hhea(
                    otf_filestream);
            /*std::cout << " - " << font_table_hhea << std::endl;*/

        } else if (t.table_tag == "cmap") {
            m_font_table_cmap =
                font::detail::otf_font::cmap::FontTableCmap::read_font_table_cmap(
                    otf_filestream);

            /*std::cout << " - " << font_table_cmap << std::endl;*/
            /*for (const auto &record : *font_table_cmap.encoding_records) {*/
            /*    std::cout << " -- " << record << std::endl;*/
            /*}*/

        } else if (t.table_tag ==
                   "CFF ") { // Note the whitespace at the end of the string
            auto m_font_table_cff =
                font::detail::otf_font::cff::FontTableCFF::read_font_table_cff(
                    otf_filestream);
            glyphs = std::move(m_font_table_cff.glyphs);
        }
    }
}

size_t font::OTFFont::glyph_index(const Unicode &unicode) const {
    return m_font_table_cmap.get_glyph_id(unicode[0]);
}

uint32_t font::OTFFont::calculate_table_checksum(const std::vector<uint8_t> &table) {
    uint32_t sum = 0;
    size_t length = table.size();

    // Process the table in 4-byte (32-bit) chunks
    for (size_t i = 0; i + 3 < length; i += 4) {
        uint32_t value = (static_cast<uint32_t>(table[i]) << 24) |
                         (static_cast<uint32_t>(table[i + 1]) << 16) |
                         (static_cast<uint32_t>(table[i + 2]) << 8) |
                         static_cast<uint32_t>(table[i + 3]);
        sum += value;
    }

    // Handle remaining bytes (if table size is not a multiple of 4)
    size_t remainder = length % 4;
    if (remainder > 0) {
        uint32_t value = 0;
        for (size_t i = length - remainder; i < length; i++) {
            value = (value << 8) | static_cast<uint32_t>(table[i]);
        }
        // Pad with zeros to make it 32-bit
        value <<= (4 - remainder) * 8;
        sum += value;
    }

    return sum;
}

bool font::OTFFont::validate_table(std::ifstream &stream,
                                   const font::detail::otf_font::TableRecord &record) {
    stream.seekg(record.offset);
    std::vector<uint8_t> table = font::detail::read_n_bytes(stream, record.length);
    stream.seekg(record.offset); // reset filestream pointer

    if (record.table_tag == "head") {
        // Zero out the checkSumAdjustment field (bytes 8-11) before validation
        if (table.size() >= 12) {
            table[8] = table[9] = table[10] = table[11] = 0;
        }
    }

    const uint32_t checksum = calculate_table_checksum(table);
    return checksum == record.checksum;
}
