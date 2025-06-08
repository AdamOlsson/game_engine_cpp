#include "render_engine/ui/Menu.h"
#include "render_engine/ui/Button.h"
#include "render_engine/ui/ElementProperties.h"
#include <vector>

using namespace ui;

Menu::Menu(Button &button) : m_button(button) {}
Menu::Menu(Button &&button) : m_button(std::move(button)) {}

Menu &Menu::add_button(Button &&button) {
    submenus.push_back(std::nullopt);
    buttons.push_back(button);
    num_items = buttons.size();
    return *this;
}

Menu &Menu::add_button(Button &button) { return add_button(std::move(button)); }

Menu &Menu::add_submenu(Menu &&menu) {
    submenus.push_back(menu);
    buttons.push_back(std::nullopt);
    num_items = buttons.size();
    return *this;
}

Menu &Menu::add_submenu(Menu &menu) { return add_submenu(std::move(menu)); }

std::vector<Button *> Menu::button_iterator() {
    return MenuButtonIterator(submenus, buttons).items;
}

std::vector<ElementProperties *> Menu::properties_iterator() {
    return MenuPropertiesIterator(submenus, buttons).items;
}

Menu::MenuButtonIterator::MenuButtonIterator(
    std::vector<std::optional<Menu>> &submenus,
    std::vector<std::optional<Button>> &buttons) {
    if (submenus.size() != buttons.size()) {
        throw std::runtime_error("Menu and Button vectors need to be same size");
    }
    size_t num_items = buttons.size();
    items.reserve(num_items);
    for (auto i = 0; i < num_items; i++) {
        if (submenus[i].has_value()) {
            items.push_back(&submenus[i]->m_button);
        } else if (buttons[i].has_value()) {
            items.push_back(&buttons[i].value());
        }
    }
}

Menu::MenuPropertiesIterator::MenuPropertiesIterator(
    std::vector<std::optional<Menu>> &submenus,
    std::vector<std::optional<Button>> &buttons) {
    if (submenus.size() != buttons.size()) {
        throw std::runtime_error("Menu and Button vectors need to be same size");
    }
    size_t num_items = buttons.size();
    items.reserve(num_items);
    for (auto i = 0; i < num_items; i++) {
        if (submenus[i].has_value()) {
            items.push_back(&submenus[i]->m_button.properties);
        } else if (buttons[i].has_value()) {
            items.push_back(&buttons[i]->properties);
        }
    }
}

// namespace ui
