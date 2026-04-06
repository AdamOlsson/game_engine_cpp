#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "Tileset.h"

namespace tiled {

class TilesetLoader {
  public:
    static Tileset load_from_file(const std::filesystem::path &tileset_path,
                                  const std::filesystem::path &base_path);

  private:
    static std::string extract_attribute(const std::string &element,
                                         const std::string &attribute);
    static std::vector<uint8_t> read_file_bytes(const std::string &path);
};

} // namespace tiled
