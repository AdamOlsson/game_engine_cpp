#pragma once

#include <optional>
#include <string_view>
#include <unordered_map>
namespace font {
class GlyphNames {
  public:
    GlyphNames() = delete;
    GlyphNames(const GlyphNames &) = delete;
    GlyphNames &operator=(const GlyphNames &) = delete;

    [[nodiscard]] static std::optional<char>
    to_char(std::string_view glyphName) noexcept {
        if (auto it = glyphMap.find(glyphName); it != glyphMap.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    // Convert glyph name to character with default fallback
    [[nodiscard]] static constexpr char to_char_or(std::string_view glyphName,
                                                   char defaultChar) noexcept {
        return to_char(glyphName).value_or(defaultChar);
    }

  private:
    static inline const std::unordered_map<std::string_view, char> glyphMap = {
        {".notdef", ' '},
        // Basic Latin punctuation
        {"exclam", '!'},
        {"quotedbl", '"'},
        {"numbersign", '#'},
        {"dollar", '$'},
        {"percent", '%'},
        {"ampersand", '&'},
        {"quotesingle", '\''},
        {"parenleft", '('},
        {"parenright", ')'},
        {"asterisk", '*'},
        {"plus", '+'},
        {"comma", ','},
        {"hyphen", '-'},
        {"period", '.'},
        {"slash", '/'},

        // Digits
        {"zero", '0'},
        {"one", '1'},
        {"two", '2'},
        {"three", '3'},
        {"four", '4'},
        {"five", '5'},
        {"six", '6'},
        {"seven", '7'},
        {"eight", '8'},
        {"nine", '9'},

        // More punctuation
        {"colon", ':'},
        {"semicolon", ';'},
        {"less", '<'},
        {"equal", '='},
        {"greater", '>'},
        {"question", '?'},
        {"at", '@'},

        // Brackets
        {"bracketleft", '['},
        {"backslash", '\\'},
        {"bracketright", ']'},
        {"asciicircum", '^'},
        {"underscore", '_'},
        {"grave", '`'},
        {"braceleft", '{'},
        {"bar", '|'},
        {"braceright", '}'},
        {"tilde", '~'},

        // Space
        {"space", ' '},
    };
};
} // namespace font
