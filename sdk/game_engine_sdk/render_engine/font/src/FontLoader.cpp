#include "font/FontLoader.h"
#include "OutlineBuilder.h"
#include <format>
#include <freetype/freetype.h>
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
    m_format = font::format::from_string(font_format);
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

    if (int err = FT_Load_Glyph(m_face, gid, FT_LOAD_NO_BITMAP)) {
        throw std::runtime_error(std::format(
            "Error: Failed to load glyph id {} with error number {}.", gid, err));
    }

    if (m_face->glyph->format != FT_GLYPH_FORMAT_OUTLINE) {
        throw std::runtime_error(std::format("Error: Not an outline glyph."));
    }

    FT_Outline *outline = &m_face->glyph->outline;

    FontFill fill;
    switch (FT_Outline_Get_Orientation(outline)) {
    case FT_ORIENTATION_FILL_LEFT:
        fill = FontFill::Left;
        break;
    case FT_ORIENTATION_FILL_RIGHT:
        fill = FontFill::Right;
        break;
    case FT_ORIENTATION_NONE:
        fill = FontFill::Unkown;
        break;
    default:
        break;
    }

    // Swap the y-axis to point downward for rendering
    FT_Matrix matrix;
    matrix.xx = 0x10000; // 1.0 in 16.16 fixed point
    matrix.xy = 0;
    matrix.yx = 0;
    matrix.yy = -0x10000; // -1.0 in 16.16 fixed point
    FT_Outline_Transform(outline, &matrix);

    OutlineBuilder builder;
    FT_Outline_Decompose(outline, builder.funcs(), &builder);

    return GlyphOutlines{
        .line_segments = std::move(builder.line_segments),
        .quadratic_curves = std::move(builder.quadratic_curves),
        .fill = fill,
    };
}

font::GlyphMetrics
font::FontLoader::get_glyph_metrics(const font::Unicode &codepoint) const {
    return get_glyph_metrics(get_glyph_index(codepoint));
}

font::GlyphMetrics font::FontLoader::get_glyph_metrics(const char32_t gid) const {
    return get_glyph_metrics(get_glyph_index(gid));
}

font::GlyphMetrics font::FontLoader::get_glyph_metrics(const unsigned int gid) const {

    if (gid == 0) {
        return GlyphMetrics{};
    }

    if (int err = FT_Load_Glyph(m_face, gid, FT_LOAD_NO_BITMAP)) {
        throw std::runtime_error(std::format(
            "Error: Failed to load glyph id {} with error number {}.", gid, err));
    }

    const auto metrics = m_face->glyph->metrics;
    return GlyphMetrics{
        .width = metrics.width,
        .height = metrics.height,
        .hori_bearing_x = metrics.horiBearingX,
        .hori_bearing_y = metrics.horiBearingY,
        .hori_advance = metrics.horiAdvance,
        .vert_bearing_x = metrics.vertBearingX,
        .vert_bearing_y = metrics.vertBearingY,
        .vert_advance = metrics.vertAdvance,
    };
}

font::GlyphAdvance
font::FontLoader::get_glyph_advance(const font::Unicode &codepoint) const {
    return get_glyph_advance(get_glyph_index(codepoint));
}

font::GlyphAdvance font::FontLoader::get_glyph_advance(const char32_t gid) const {
    return get_glyph_advance(get_glyph_index(gid));
}

font::GlyphAdvance font::FontLoader::get_glyph_advance(const unsigned int gid) const {

    if (gid == 0) {
        return GlyphAdvance{};
    }

    if (int err = FT_Load_Glyph(m_face, gid, FT_LOAD_NO_BITMAP)) {
        throw std::runtime_error(std::format(
            "Error: Failed to load glyph id {} with error number {}.", gid, err));
    }

    const auto advance = m_face->glyph->advance;
    return GlyphAdvance{
        .x = advance.x,
        .y = advance.y,
    };
}

font::GlyphKerning
font::FontLoader::get_glyph_kerning(const font::Unicode &codepoint1,
                                    const font::Unicode &codepoint2) const {

    return get_glyph_kerning(get_glyph_index(codepoint1), get_glyph_index(codepoint2));
}

font::GlyphKerning font::FontLoader::get_glyph_kerning(const char32_t gid1,
                                                       const char32_t gid2) const {

    return get_glyph_kerning(get_glyph_index(gid1), get_glyph_index(gid2));
}

font::GlyphKerning font::FontLoader::get_glyph_kerning(const unsigned int gid1,
                                                       const unsigned int gid2) const {

    if (gid1 == 0 || gid2 == 0) {
        return GlyphKerning{};
    }

    FT_Vector kerning;
    if (int err = FT_Get_Kerning(m_face, gid1, gid2, FT_KERNING_UNSCALED, &kerning)) {
        throw std::runtime_error(
            std::format("Error: Failed to get kerning information for glyph id {} and "
                        "glyph id {} with error number {}.",
                        gid1, gid2, err));
    }

    return GlyphKerning{.x = kerning.x, .y = kerning.y};
}

signed long font::FontLoader::get_num_glyphs() const { return m_face->num_glyphs; }

font::FontFormat font::FontLoader::get_format() const { return m_format; }

font::FontBBox font::FontLoader::get_font_bbox() const {
    return FontBBox{
        .x_min = static_cast<int>(m_face->bbox.xMin),
        .y_min = static_cast<int>(m_face->bbox.yMin),
        .x_max = static_cast<int>(m_face->bbox.xMax),
        .y_max = static_cast<int>(m_face->bbox.yMax),
    };
}

unsigned short font::FontLoader::get_units_per_em() const { return m_face->units_per_EM; }

std::ostream &operator<<(std::ostream &os, const font::FontBBox &bbox) {
    os << bbox.to_string();
    return os;
}
