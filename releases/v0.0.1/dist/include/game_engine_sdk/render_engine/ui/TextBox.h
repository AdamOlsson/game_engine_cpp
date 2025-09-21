#pragma once

#include "game_engine_sdk/render_engine/ui/ElementProperties.h"
#include <string>
namespace ui {

class TextBox {
  public:
    std::string text;
    ElementProperties properties;

    TextBox() = default;
    TextBox(std::string &&text, ui::ElementProperties &&properties);

    ~TextBox() = default;

    TextBox(TextBox &&other) noexcept = default;
    TextBox(const TextBox &other) = default;

    TextBox &operator=(TextBox &&other) noexcept = default;
    TextBox &operator=(const TextBox &other) = default;
};

}; // namespace ui
