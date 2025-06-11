#include "render_engine/ui/Menu.h"
#include "render_engine/ui/Button.h"
#include "render_engine/ui/ElementProperties.h"
#include "render_engine/ui/UI.h"
#include <iostream>
#include <vector>

using namespace ui;

// This constructor will only be called when we create a submenu
Menu::Menu(Button &button, Button &back_button)
    : m_ui(nullptr), m_button(button), m_back_button(back_button) {}

// This constructor will only be called when we create a submenu
Menu::Menu(Button &&button, Button &&back_button)
    : m_ui(nullptr), m_button(std::move(button)), m_back_button(std::move(back_button)) {}

void Menu::setup_navigation_callback() {
    // Setup navigation to this submenu
    auto user_on_click = m_button.on_click;
    m_button = m_button.set_on_click([this, user_on_click](ui::Button &self) {
        user_on_click(self);
        if (this->m_ui != nullptr) {
            this->m_ui->push_new_menu(this);
        } else {
            std::cout << "WARNING! Clicking menu button without linking. Call link() "
                         "from the UI constructor."
                      << std::endl;
        }
    });

    // Setup navigation to parent menu
    auto user_on_click_back = m_back_button->on_click;
    m_back_button =
        m_back_button->set_on_click([this, user_on_click_back](ui::Button &self) {
            user_on_click_back(self);
            if (this->m_ui != nullptr) {
                this->m_ui->pop_menu();
            } else {
                std::cout << "WARNING! Clicking menu button without linking. Call link() "
                             "from the UI constructor."
                          << std::endl;
            }
        });
}

Menu &Menu::add_button(Button &&button) {
    m_submenus.push_back(std::nullopt);
    m_buttons.push_back(std::move(button));
    return *this;
}

Menu &Menu::add_button(Button &button) { return add_button(std::move(button)); }

Menu &Menu::add_submenu(Menu &&submenu) {
    m_submenus.push_back(std::move(submenu));
    m_buttons.push_back(std::nullopt);
    return *this;
}

Menu &Menu::add_submenu(Menu &menu) { return add_submenu(std::move(menu)); }

void Menu::link(UI *ui) {
    // This functions is required because of the design decision to not use unique
    // pointers. Because we do a lot of moving anc copying of memory address during
    // creation of a menu, we can't create a reliable pointer until everything has
    // settled. This would be solved by using unique pointers. However, I want a nice way
    // to abstract the unique pointers behind some class to prevent std::unique spamming
    // for the end user
    m_ui = ui;
    setup_navigation_callback();
    for (auto &menu : m_submenus) {
        if (menu.has_value()) {
            menu->link(ui);
        }
    }
    update_vectors();
}

std::vector<Button *> Menu::create_button_vector() {
    auto button_vec = MenuButtonIterator(m_submenus, m_buttons).items;
    if (m_back_button.has_value()) {
        button_vec.push_back(&m_back_button.value());
    }
    return button_vec;
}

std::vector<ElementProperties *> Menu::create_properties_vector() {
    auto prop_vec = MenuPropertiesIterator(m_submenus, m_buttons).items;
    if (m_back_button.has_value()) {
        prop_vec.push_back(&m_back_button.value().properties);
    }
    return prop_vec;
}

void Menu::update_vectors() {
    button_vector = create_button_vector();
    properties_vector = create_properties_vector();
}

Menu::MenuButtonIterator::MenuButtonIterator(
    std::vector<std::optional<Menu>> &submenus,
    std::vector<std::optional<Button>> &buttons) {
    if (submenus.size() != buttons.size()) {
        throw std::runtime_error(
            "MenuButtonIterator: Menu and Button vectors need to be same size");
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
        throw std::runtime_error(
            "MenuPropertiesIterator: Menu and Button vectors need to be same size");
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
