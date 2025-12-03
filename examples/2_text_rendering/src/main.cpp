#include "font/detail/ifstream_util.h"
#include "font/detail/otf_font/FontTableCmap.h"
#include "font/detail/otf_font/FontTableHead.h"
#include "font/detail/otf_font/FontTableHhea.h"
#include "font/detail/otf_font/SfntHeader.h"
#include "font/detail/otf_font/cff/FontTableCFF.h"

#include <iostream>

#define ASSET_FILE(filename) ASSET_DIR "/" filename

// OTF Data types:
// https://learn.microsoft.com/en-us/typography/opentype/spec/otff#data-types

int main() {
    /*auto otf_filestream = open_filestream(ASSET_FILE("ftystrategycidencv.otf"));*/
    /*auto otf_filestream = open_filestream(ASSET_FILE("dustismo-roman-italic.ttf"));*/
    auto otf_filestream = font::detail::open_filestream(
        ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf"));

    const auto sfnt_header =
        font::detail::otf_font::SFntHeader::read_sfnt_header(otf_filestream);
    std::cout << sfnt_header << std::endl;

    std::cout << "Found tables:" << std::endl;
    for (const auto &t : *sfnt_header.table_records) {

        if (!otf_filestream) {
            std::cerr << "Stream error after read: " << otf_filestream.rdstate()
                      << std::endl;
        }

        otf_filestream.seekg(t.offset);
        std::cout << "Seeking to " << t.offset << " (0x" << std::hex << t.offset << ")"
                  << std::endl;
        /*std::cout << " # " << t.table_tag << std::endl;*/

        if (t.table_tag == "head") {
            // TODO: Calculate checksum of TableRecord
            const auto font_table_head =
                font::detail::otf_font::FontTableHead::read_font_table_head(
                    otf_filestream);
            /*std::cout << " - " << font_table_head << std::endl;*/

        } else if (t.table_tag == "hhea") {
            const auto font_table_hhea =
                font::detail::otf_font::FontTableHhea::read_font_table_hhea(
                    otf_filestream);
            /*std::cout << " - " << font_table_hhea << std::endl;*/

        } else if (t.table_tag == "cmap") {
            const auto font_table_cmap =
                font::detail::otf_font::cmap::FontTableCmap::read_font_table_cmap(
                    otf_filestream);
            /*std::cout << " - " << font_table_cmap << std::endl;*/
            for (const auto &record : *font_table_cmap.encoding_records) {
                /*std::cout << " -- " << record << std::endl;*/
            }

        } else if (t.table_tag ==
                   "CFF ") { // Note the whitespace at the end of the string
            const auto font_table_cff =
                font::detail::otf_font::cff::FontTableCFF::read_font_table_cff(
                    otf_filestream);
            /*std::cout << " - " << font_table_cff << std::endl;*/
        }
    }

    return 0;
}
