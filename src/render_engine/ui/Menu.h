#pragma once
#include "render_engine/ui/Button.h"
#include "render_engine/ui/ElementProperties.h"
#include <vector>

namespace ui {

class UI;

class Menu {
  private:
    class MenuButtonIterator {
      public:
        std::vector<Button *> items;
        MenuButtonIterator(std::vector<std::optional<Menu>> &submenus,
                           std::vector<std::optional<Button>> &buttons);
    };

    class MenuPropertiesIterator {
      public:
        std::vector<ElementProperties *> items;
        MenuPropertiesIterator(std::vector<std::optional<Menu>> &submenus,
                               std::vector<std::optional<Button>> &buttons);
    };

    ui::UI *m_ui;
    ui::Button m_button;
    std::optional<ui::Button> m_back_button;

    std::vector<std::optional<Menu>> submenus;
    std::vector<std::optional<Button>> buttons;

    void setup_navigation_callback();
    void update_vectors();

  public:
    std::vector<ElementProperties *> create_properties_vector();
    std::vector<Button *> create_button_vector();

    std::vector<Button *> button_vector;
    std::vector<ElementProperties *> properties_vector;
    /**
     * @brief Creates a menu which serves as the top level menu
     *
     * @since C++20
     */
    Menu() = default;

    Menu(Menu &&other) = default;
    Menu(const Menu &other) = default;

    Menu &operator=(Menu &&other) = default;
    Menu &operator=(const Menu &other) = default;

    /**
     * @brief Creates a menu which serves as a submenu
     *
     * The menu can contain buttons or additional menus. The menu items handle
     * logic for transitioning up and down menus.
     *
     * @param button The button which, once pressed, will display the submenu
     *
     * @see Button
     * @since C++20
     */
    Menu(Button &button, Button &back_button);
    Menu(Button &&button, Button &&back_button);

    Menu &add_button(Button &&button);
    Menu &add_button(Button &button);
    Menu &add_submenu(Menu &&menu);
    Menu &add_submenu(Menu &menu);

    /**
     * @brief Links submenus to which UI they belong to.
     *
     * In order to handle menu nagivation, the UI constructor needs to call this function.
     *
     * @param ui The UI for which the menu belong to
     *
     * @since C++20
     */
    void link(UI *ui);
};

} // namespace ui
