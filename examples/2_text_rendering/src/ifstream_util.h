#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
std::ifstream open_filestream(const std::string &filename);

uint32_t read_uint32(std::ifstream &stream);

int64_t read_int64(std::ifstream &stream);

uint64_t read_uint64(std::ifstream &stream);

int16_t read_int16(std::ifstream &stream);

uint16_t read_uint16(std::ifstream &stream);

uint8_t read_uint8(std::ifstream &stream);

std::vector<char> read_n_bytes(std::ifstream &stream, const size_t n);

std::unique_ptr<std::vector<uint8_t>> read_n_bytes_unique(std::ifstream &stream,
                                                          const size_t n);

template <typename It> void print_iter(It begin, It end) {
    for (It it = begin; it != end; ++it) {
        std::cout << *it;
    }
}
