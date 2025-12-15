#include "font/detail/ifstream_util.h"

std::ifstream font::detail::open_filestream(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error(std::format("Failed to open file {}", filename));
    }

    file.seekg(0);
    return file;
}

uint32_t font::detail::read_uint32(std::ifstream &stream) {
    uint32_t value = 0;
    value |= stream.get() << 24;
    value |= stream.get() << 16;
    value |= stream.get() << 8;
    value |= stream.get();
    return value;
}

int64_t font::detail::read_int64(std::ifstream &stream) {
    int64_t value = 0;
    value |= read_uint32(stream);
    value = value << 32;
    value |= read_uint32(stream);
    return value;
}

uint64_t font::detail::read_uint64(std::ifstream &stream) {
    uint64_t value = 0;
    value |= read_uint32(stream);
    value = value << 32;
    value |= read_uint32(stream);
    return value;
}

int16_t font::detail::read_int16(std::ifstream &stream) {
    int16_t value = 0;
    value |= stream.get() << 8;
    value |= stream.get();
    return value;
}

uint16_t font::detail::read_uint16(std::ifstream &stream) {
    uint16_t value = 0;
    value |= stream.get() << 8;
    value |= stream.get();
    return value;
}

uint8_t font::detail::read_uint8(std::ifstream &stream) { return stream.get(); }

std::vector<uint8_t> font::detail::read_n_bytes(std::ifstream &stream, const size_t n) {
    std::vector<uint8_t> buffer(n);
    stream.read(reinterpret_cast<char *>(buffer.data()), n);
    return buffer;
}

std::unique_ptr<std::vector<uint8_t>>
font::detail::read_n_bytes_unique(std::ifstream &stream, const size_t n) {
    auto buffer = std::make_unique<std::vector<uint8_t>>(n);
    stream.read(reinterpret_cast<char *>(buffer->data()), n);
    return buffer;
}

std::vector<uint16_t> font::detail::read_n_uint16(std::ifstream &stream, const size_t n) {
    std::vector<uint16_t> buffer;
    buffer.reserve(n);
    for (auto i = 0; i < n; i++) {
        buffer.push_back(read_uint16(stream));
    }
    return buffer;
}

std::vector<int16_t> font::detail::read_n_int16(std::ifstream &stream, const size_t n) {
    std::vector<int16_t> buffer;
    buffer.reserve(n);
    for (auto i = 0; i < n; i++) {
        buffer.push_back(read_int16(stream));
    }
    return buffer;
}
