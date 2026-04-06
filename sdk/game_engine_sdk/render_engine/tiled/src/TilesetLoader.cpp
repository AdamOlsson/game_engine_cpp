#include "TilesetLoader.h"
#include <fstream>
#include <sstream>

namespace tiled {

std::vector<uint8_t> TilesetLoader::read_file_bytes(const std::string &path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        return {};
    }

    size_t file_size = static_cast<size_t>(file.tellg());
    std::vector<uint8_t> buffer(file_size);
    file.seekg(0);
    file.read(reinterpret_cast<char *>(buffer.data()), file_size);
    file.close();
    return buffer;
}

std::string TilesetLoader::extract_attribute(const std::string &element,
                                             const std::string &attribute) {
    std::string search = attribute + "=\"";
    auto pos = element.find(search);
    if (pos == std::string::npos) {
        return "";
    }
    pos += search.size();
    auto end = element.find("\"", pos);
    if (end == std::string::npos) {
        return "";
    }
    return element.substr(pos, end - pos);
}

Tileset TilesetLoader::load_from_file(const std::filesystem::path &tileset_path,
                                      const std::filesystem::path &base_path) {
    std::ifstream file(tileset_path.string());
    if (!file.is_open()) {
        return {};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    auto tileset_start = content.find("<tileset");
    if (tileset_start == std::string::npos) {
        return {};
    }

    auto tileset_end = content.find(">", tileset_start);
    if (tileset_end == std::string::npos) {
        return {};
    }

    std::string tileset_element =
        content.substr(tileset_start, tileset_end - tileset_start + 1);

    int32_t tile_width = std::stoi(extract_attribute(tileset_element, "tilewidth"));
    int32_t tile_height = std::stoi(extract_attribute(tileset_element, "tileheight"));
    int32_t tilecount = std::stoi(extract_attribute(tileset_element, "tilecount"));
    int32_t columns = std::stoi(extract_attribute(tileset_element, "columns"));

    int32_t image_width = 0;
    int32_t image_height = 0;
    std::vector<uint8_t> image_data;

    auto image_start = content.find("<image", tileset_end);
    if (image_start != std::string::npos) {
        auto image_end = content.find(">", image_start);
        if (image_end != std::string::npos) {
            std::string image_element =
                content.substr(image_start, image_end - image_start + 1);

            std::string source = extract_attribute(image_element, "source");
            image_width = std::stoi(extract_attribute(image_element, "width"));
            image_height = std::stoi(extract_attribute(image_element, "height"));

            std::filesystem::path image_path = base_path / source;
            image_data = read_file_bytes(image_path.string());
        }
    }

    return Tileset(tile_width, tile_height, tilecount, columns, std::move(image_data),
                   image_width, image_height);
}

} // namespace tiled
