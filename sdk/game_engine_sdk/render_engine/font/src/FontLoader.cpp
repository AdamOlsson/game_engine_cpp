#include "font/FontLoader.h"
#include "OutlineBuilder.h"
#include <format>
#include FT_FONT_FORMATS_H

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
    } else if (error == FT_Err_Cannot_Open_Resource) {
        throw std::runtime_error(
            std::format("Error: Can't open font file {}.", filepath));
    } else if (error) {
        throw std::runtime_error(
            std::format("Error: Failed to create font face from {} with error code {}.",
                        filepath, error));
    }

    const auto font_format = FT_Get_Font_Format(m_face);
    m_format = font_format::from_string(font_format);
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
            std::format("Error: Failed to create font face with error code {}.", error));
    }
};

font::FontLoader::FontLoader(FontLoader &&other) noexcept
    : m_library(other.m_library), m_face(other.m_face) {
    other.m_library = nullptr;
    other.m_face = nullptr;
}

font::FontLoader &font::FontLoader::operator=(FontLoader &&other) noexcept {
    if (this != &other) {
        if (m_face) {
            FT_Done_Face(m_face);
        }

        if (m_library) {
            FT_Done_FreeType(m_library);
        }

        m_library = other.m_library;
        m_face = other.m_face;
        other.m_library = nullptr;
        other.m_face = nullptr;
    }
    return *this;
}

font::FontLoader::~FontLoader() {
    if (m_face) {
        FT_Done_Face(m_face);
    }

    if (m_library) {
        FT_Done_FreeType(m_library);
    }
};

unsigned int font::FontLoader::get_glyph_index(const font::Unicode &codepoint) const {
    return FT_Get_Char_Index(m_face, codepoint.first());
}

std::array<char, 256>
font::FontLoader::get_glyph_name(const font::Unicode &codepoint) const {
    return get_glyph_name(get_glyph_index(codepoint.first()));
}

std::array<char, 256> font::FontLoader::get_glyph_name(const unsigned int gid) const {
    std::array<char, 256> name;
    name.fill(0);
    int error = FT_Get_Glyph_Name(m_face, gid, name.data(), 256);
    if (error) {
        throw std::runtime_error(std::format("Error: Failed to get name for glyph."));
    }
    return name;
}

font::GlyphOutlines
font::FontLoader::get_glyph_outline(const font::Unicode &codepoint) const {
    return get_glyph_outline(get_glyph_index(codepoint));
}

font::GlyphOutlines font::FontLoader::get_glyph_outline(const unsigned int gid) const {
    if (gid == 0) {
        return {};
    }

    if (FT_Load_Glyph(m_face, gid, FT_LOAD_NO_BITMAP)) {
        throw std::runtime_error(std::format("Error: Failed to load glyph."));
    }

    if (m_face->glyph->format != FT_GLYPH_FORMAT_OUTLINE) {
        throw std::runtime_error(std::format("Error: Not an outline glyph."));
    }

    FT_Outline *outline = &m_face->glyph->outline;

    // Swap the y-axis to point downward for rendering
    FT_Matrix matrix;
    matrix.xx = 0x10000; // 1.0 in 16.16 fixed point
    matrix.xy = 0;
    matrix.yx = 0;
    matrix.yy = -0x10000; // -1.0 in 16.16 fixed point
    FT_Outline_Transform(outline, &matrix);

    OutlineBuilder builder(m_format);
    FT_Outline_Decompose(outline, builder.funcs(), &builder);

    return GlyphOutlines{
        .line_segments = std::move(builder.line_segments),
        .quadratic_curves = std::move(builder.quadratic_curves),
    };
}

signed long font::FontLoader::get_num_glyphs() { return m_face->num_glyphs; }

font::FontFormat font::FontLoader::get_format() { return m_format; }
