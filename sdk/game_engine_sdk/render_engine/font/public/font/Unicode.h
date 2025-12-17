#include <cstdint>
#include <string>
#include <vector>
namespace font {
class Unicode {
    std::vector<char32_t> codepoints;

    // UTF-8 decoder
    static std::vector<char32_t> decode_utf8(const char *str) {
        std::vector<char32_t> result;

        while (*str) {
            uint8_t byte1 = static_cast<uint8_t>(*str);
            char32_t cp = 0;

            if ((byte1 & 0x80) == 0) { // 1-byte (ASCII)
                cp = byte1;
                str += 1;
            } else if ((byte1 & 0xE0) == 0xC0) { // 2-byte
                cp = ((byte1 & 0x1F) << 6) | (str[1] & 0x3F);
                str += 2;
            } else if ((byte1 & 0xF0) == 0xE0) { // 3-byte
                cp = ((byte1 & 0x0F) << 12) | ((str[1] & 0x3F) << 6) | (str[2] & 0x3F);
                str += 3;
            } else if ((byte1 & 0xF8) == 0xF0) { // 4-byte
                cp = ((byte1 & 0x07) << 18) | ((str[1] & 0x3F) << 12) |
                     ((str[2] & 0x3F) << 6) | (str[3] & 0x3F);
                str += 4;
            } else {
                // Invalid UTF-8, skip byte
                str += 1;
                continue;
            }

            result.push_back(cp);
        }

        return result;
    }

  public:
    Unicode(const char *str) : codepoints(decode_utf8(str)) {}

    Unicode(const std::string &str) : codepoints(decode_utf8(str.c_str())) {}

    Unicode(char32_t cp) : codepoints{cp} {}

    Unicode(const char32_t *str) {
        while (*str) {
            codepoints.push_back(*str++);
        }
    }

    char32_t first() const { return codepoints.empty() ? 0 : codepoints[0]; }

    const std::vector<char32_t> &all() const { return codepoints; }

    size_t size() const { return codepoints.size(); }

    auto begin() const { return codepoints.begin(); }
    auto end() const { return codepoints.end(); }

    char32_t operator[](size_t index) const { return codepoints[index]; }
};
} // namespace font
