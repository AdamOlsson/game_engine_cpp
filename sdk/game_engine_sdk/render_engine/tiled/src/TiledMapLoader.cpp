#include "TiledMapLoader.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

namespace tiled {

namespace {

std::string trim(const std::string &str) {
    auto start = std::find_if(str.begin(), str.end(),
                              [](unsigned char ch) { return !std::isspace(ch); });
    auto end = std::find_if(str.rbegin(), std::reverse_iterator(str.end()),
                            [](unsigned char ch) { return !std::isspace(ch); })
                   .base();
    return (start < end) ? std::string(start, end) : std::string();
}

std::string to_lowercase(const std::string &str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string extract_filename(const std::string &path) {
    auto last_slash = path.find_last_of("/\\");
    if (last_slash == std::string::npos) {
        return path;
    }
    return path.substr(last_slash + 1);
}

} // namespace

TiledMapLoader::TiledMapLoader(const std::filesystem::path &path) { parse_file(path); }

std::string TiledMapLoader::get_tileset_basename() const { return m_tileset_basename; }

std::filesystem::path TiledMapLoader::get_tileset_path() const { return m_tileset_path; }

RenderOrder TiledMapLoader::get_render_order() const { return m_render_order; }

int32_t TiledMapLoader::get_tile_width() const { return m_tile_width; }

int32_t TiledMapLoader::get_tile_height() const { return m_tile_height; }

size_t TiledMapLoader::get_layer_count() const { return m_layers.size(); }

const Layer &TiledMapLoader::get_layer(size_t index) const { return m_layers[index]; }

const Chunk &TiledMapLoader::get_chunk(size_t layer_index, size_t chunk_index) const {
    return m_layers[layer_index][chunk_index];
}

uint32_t TiledMapLoader::get_tile(size_t layer_index, size_t chunk_index,
                                  size_t tile_index) const {
    return m_layers[layer_index][chunk_index].tiles[tile_index];
}

void TiledMapLoader::parse_file(const std::filesystem::path &path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    parse_map_element(content, path);
}

void TiledMapLoader::parse_map_element(const std::string &content,
                                       const std::filesystem::path &map_path) {
    auto map_start = content.find("<map");
    if (map_start == std::string::npos) {
        return;
    }

    auto map_end = content.find(">", map_start);
    if (map_end == std::string::npos) {
        return;
    }

    std::string map_element = content.substr(map_start, map_end - map_start + 1);

    std::string renderorder = extract_attribute(map_element, "renderorder");
    if (!renderorder.empty()) {
        std::string lower = to_lowercase(renderorder);
        if (lower == "right-down") {
            m_render_order = RenderOrder::RightDown;
        } else if (lower == "right-up") {
            m_render_order = RenderOrder::RightUp;
        } else if (lower == "left-down") {
            m_render_order = RenderOrder::LeftDown;
        } else if (lower == "left-up") {
            m_render_order = RenderOrder::LeftUp;
        }
    }

    std::string tilewidth = extract_attribute(map_element, "tilewidth");
    if (!tilewidth.empty()) {
        m_tile_width = std::stoi(tilewidth);
    }

    std::string tileheight = extract_attribute(map_element, "tileheight");
    if (!tileheight.empty()) {
        m_tile_height = std::stoi(tileheight);
    }

    auto tileset_start = content.find("<tileset", map_end);
    if (tileset_start != std::string::npos) {
        auto tileset_end = content.find(">", tileset_start);
        if (tileset_end != std::string::npos) {
            std::string tileset_element =
                content.substr(tileset_start, tileset_end - tileset_start + 1);
            std::string source = extract_attribute(tileset_element, "source");
            if (!source.empty()) {
                m_tileset_basename = extract_filename(source);

                std::filesystem::path base_dir = map_path.parent_path();
                m_tileset_path = base_dir / source;
            }
        }
    }

    size_t pos = map_end;
    while (true) {
        auto layer_start = content.find("<layer", pos);
        if (layer_start == std::string::npos) {
            break;
        }

        auto layer_end = content.find(">", layer_start);
        if (layer_end == std::string::npos) {
            break;
        }

        std::string layer_element =
            content.substr(layer_start, layer_end - layer_start + 1);
        std::string layer_name = extract_attribute(layer_element, "name");
        if (!layer_name.empty()) {
            m_layer_names.push_back(layer_name);
        }

        auto data_start = content.find("<data", layer_end);
        if (data_start != std::string::npos) {
            auto data_end_tag = content.find("</data>", data_start);
            if (data_end_tag != std::string::npos) {
                std::string data_content =
                    content.substr(data_start, data_end_tag - data_start);
                Layer layer = parse_chunks(data_content);
                m_layers.push_back(std::move(layer));
                pos = data_end_tag;
            } else {
                pos = layer_end;
            }
        } else {
            pos = layer_end;
        }
    }
}

void TiledMapLoader::parse_layer_element(const std::string &content) { (void)content; }

std::string TiledMapLoader::extract_attribute(const std::string &element,
                                              const std::string &attribute) {
    std::string search = attribute + "=\"";
    auto pos = element.find(search);
    if (pos == std::string::npos) {
        search = attribute + "='";
        pos = element.find(search);
        if (pos == std::string::npos) {
            return "";
        }
    }

    pos += search.size();
    auto end = element.find("\"", pos);
    if (end == std::string::npos) {
        end = element.find("'", pos);
        if (end == std::string::npos) {
            return "";
        }
    }

    return element.substr(pos, end - pos);
}

Layer TiledMapLoader::parse_chunks(const std::string &data_content) {
    Layer layer;

    size_t pos = 0;
    while (true) {
        auto chunk_start = data_content.find("<chunk", pos);
        if (chunk_start == std::string::npos) {
            break;
        }

        auto chunk_end = data_content.find(">", chunk_start);
        if (chunk_end == std::string::npos) {
            break;
        }

        std::string chunk_element =
            data_content.substr(chunk_start, chunk_end - chunk_start + 1);
        Chunk chunk = parse_single_chunk(chunk_element);

        auto next_chunk = data_content.find("<chunk", chunk_end);
        std::string chunk_data;
        if (next_chunk == std::string::npos) {
            chunk_data = data_content.substr(chunk_end + 1);
        } else {
            chunk_data = data_content.substr(chunk_end + 1, next_chunk - chunk_end - 1);
        }

        chunk.tiles = parse_csv(chunk_data);
        layer.push_back(chunk);

        pos = (next_chunk != std::string::npos) ? next_chunk : data_content.size();
    }

    return layer;
}

Chunk TiledMapLoader::parse_single_chunk(const std::string &chunk_element) {
    Chunk chunk;
    chunk.x = std::stoi(extract_attribute(chunk_element, "x"));
    chunk.y = std::stoi(extract_attribute(chunk_element, "y"));
    chunk.width = std::stoi(extract_attribute(chunk_element, "width"));
    chunk.height = std::stoi(extract_attribute(chunk_element, "height"));
    return chunk;
}

std::vector<uint32_t> TiledMapLoader::parse_csv(const std::string &csv_content) {
    std::vector<uint32_t> tiles;
    std::string cleaned;

    for (char c : csv_content) {
        if (c == ',' || c == '\n' || c == '\r' || c == '\t') {
            if (!cleaned.empty()) {
                try {
                    tiles.push_back(std::stoul(cleaned));
                } catch (...) {
                    tiles.push_back(0);
                }
                cleaned.clear();
            }
        } else if (!std::isspace(static_cast<unsigned char>(c))) {
            cleaned += c;
        }
    }

    cleaned = trim(cleaned);
    if (!cleaned.empty()) {
        try {
            tiles.push_back(std::stoul(cleaned));
        } catch (...) {
            tiles.push_back(0);
        }
    }

    return tiles;
}

} // namespace tiled
