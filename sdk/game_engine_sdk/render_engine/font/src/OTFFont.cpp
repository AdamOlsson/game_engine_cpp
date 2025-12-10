#include "font/OTFFont.h"
#include "font/detail/ifstream_util.h"
#include "font/detail/otf_font/FontTableCmap.h"
#include "font/detail/otf_font/FontTableHead.h"
#include "font/detail/otf_font/FontTableHhea.h"
#include "font/detail/otf_font/SfntHeader.h"
#include "font/detail/otf_font/cff/FontTableCFF.h"

font::OTFFont::OTFFont(const std::string &filepath) {

    auto otf_filestream = font::detail::open_filestream(filepath);
    m_sfnt_header = font::detail::otf_font::SFntHeader::read_sfnt_header(otf_filestream);
    /*std::cout << m_sfnt_header << std::endl;*/

    /*std::cout << "Found tables:" << std::endl;*/
    for (const auto &t : *m_sfnt_header.table_records) {

        if (!otf_filestream) {
            throw std::runtime_error(
                std::format("Stream error after read: ", otf_filestream.rdstate()));
        }

        otf_filestream.seekg(t.offset);
        /*std::cout << "Seeking to " << t.offset << " (0x" << std::hex << t.offset <<
         * ")"*/
        /*          << std::endl;*/
        /*std::cout << " # " << t.table_tag << std::endl;*/

        if (t.table_tag == "head") {
            // TODO: Calculate checksum of TableRecord
            m_font_table_head =
                font::detail::otf_font::FontTableHead::read_font_table_head(
                    otf_filestream);
            /*std::cout << " - " << m_font_table_head << std::endl;*/

        } else if (t.table_tag == "hhea") {
            m_font_table_hhea =
                font::detail::otf_font::FontTableHhea::read_font_table_hhea(
                    otf_filestream);
            /*std::cout << " - " << font_table_hhea << std::endl;*/

        } else if (t.table_tag == "cmap") {
            const auto font_table_cmap =
                font::detail::otf_font::cmap::FontTableCmap::read_font_table_cmap(
                    otf_filestream);

            const auto char_I = 73;
            const auto glyph_id = font_table_cmap.get_glyph_id(char_I);
            std::cout << "Glyph ID: " << glyph_id << std::endl;

            /*std::cout << " - " << font_table_cmap << std::endl;*/
            /*for (const auto &record : *font_table_cmap.encoding_records) {*/
            /*    std::cout << " -- " << record << std::endl;*/
            /*}*/

        } else if (t.table_tag ==
                   "CFF ") { // Note the whitespace at the end of the string
            m_font_table_cff =
                font::detail::otf_font::cff::FontTableCFF::read_font_table_cff(
                    otf_filestream);
            /*std::cout << " - " << font_table_cff << std::endl;*/
        }
    }
}
