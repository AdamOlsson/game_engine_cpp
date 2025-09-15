#include "game_engine_sdk/render_engine/ui/TextBox.h"
#include "game_engine_sdk/render_engine/ui/ElementProperties.h"

using namespace ui;

TextBox::TextBox(std::string &&text, ui::ElementProperties &&properties)
    : text(std::move(text)), properties(std::move(properties)) {}
