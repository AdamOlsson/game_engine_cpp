#pragma once
#include "game_engine_sdk/render_engine/Texture.h"

class TileSize : public glm::vec2 {
    using glm::vec2::vec2;

  public:
    TileSize() = default;
    explicit TileSize(const glm::vec2 &vec) : glm::vec2(vec) {}

    template <typename T>
    TileSize(T x, T y) : glm::vec2(static_cast<float>(x), static_cast<float>(y)) {}
};

class TilesetUVWT {
  private:
    glm::vec2 m_norm_tile_size;

  public:
    TilesetUVWT() = default;
    TilesetUVWT(Texture &tileset, TileSize size);
    ~TilesetUVWT() = default;

    glm::vec4 uvwt_for_tile_at(unsigned int x, unsigned y);
};
