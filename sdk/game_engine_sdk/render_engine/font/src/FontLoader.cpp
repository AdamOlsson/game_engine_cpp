#include "font/FontLoader.h"

font::FontLoader::FontLoader(const std::string &filepath) {
    int error = FT_Init_FreeType(&m_library);
    if (error) {
        throw std::runtime_error("Error: Failed to initialize FreeType library.");
    }

    const int face_index = 0;
    error = FT_New_Face(m_library, filepath.c_str(), face_index, &m_face);
    if (error == FT_Err_Unknown_File_Format) {
        throw std::runtime_error(
            std::format("Error: Can't create font face, unkown file format."));
    } else if (error) {
        throw std::runtime_error(std::format(
            "Error ({}): Failed to create font face from {}.", error, filepath));
    }
};

font::FontLoader::FontLoader(const std::vector<char> &font_data) {
    int error = FT_Init_FreeType(&m_library);
    if (error) {
        throw std::runtime_error("Error: Failed to initialize FreeType library.");
    }

    const int face_index = 0;
    error = FT_New_Memory_Face(m_library,
                               reinterpret_cast<const unsigned char *>(font_data.data()),
                               font_data.size(), face_index, &m_face);
    if (error == FT_Err_Unknown_File_Format) {
        throw std::runtime_error(
            std::format("Error: Can't create font face, unkown file format."));
    } else if (error) {
        throw std::runtime_error(
            std::format("Error ({}): Failed to create font face.", error));
    }
};

font::FontLoader::~FontLoader() {
    FT_Done_Face(m_face);
    FT_Done_FreeType(m_library);
};

unsigned int font::FontLoader::get_glyph_index(const font::Unicode &codepoint) {
    return FT_Get_Char_Index(m_face, codepoint.first());
}

void font::FontLoader::get_glyph_outline(const font::Unicode &codepoint) {

    FT_Int glyph_index = get_glyph_index(codepoint);
    if (glyph_index == 0) {
        return;
    }

    if (FT_Load_Glyph(m_face, glyph_index, FT_LOAD_NO_BITMAP)) {
        throw std::runtime_error(std::format("Error: Failed to load glyph."));
    }

    if (m_face->glyph->format != FT_GLYPH_FORMAT_OUTLINE) {
        throw std::runtime_error(std::format("Error: Not an outline glyph."));
    }

    FT_Outline *outline = &m_face->glyph->outline;
    FT_Outline_Decompose(outline, m_builder.funcs(), nullptr);
}
