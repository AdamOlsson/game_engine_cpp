#pragma once
#include "render_engine/ui/Button.h"
#include "render_engine/ui/ElementProperties.h"
#include <vector>

namespace ui {

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

    Button m_button;
    std::vector<std::optional<Menu>> submenus;
    std::vector<std::optional<Button>> buttons;

  public:
    size_t num_items;

    /**
     * @brief Creates a menu which serves as the top level menu
     *
     * @since C++20
     */
    Menu() = default;

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
    Menu(Button &button);
    Menu(Button &&button);

    Menu &add_button(Button &&button);
    Menu &add_button(Button &button);
    Menu &add_submenu(Menu &&menu);
    Menu &add_submenu(Menu &menu);

    std::vector<Button *> button_iterator();
    std::vector<ElementProperties *> properties_iterator();
};

} // namespace ui
