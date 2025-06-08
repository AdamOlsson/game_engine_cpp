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

  public:
    std::vector<std::optional<Menu>> submenus;
    std::vector<std::optional<Button>> buttons;
    size_t num_items;

    Menu() = default;

    // TODO: How can we make sure there always is a return item on the submenu? Should we
    // make sure there is a return item?
    /*Menu(std::vector<std::optional<Menu>> submenus,*/
    /*     std::vector<std::optional<Button>> buttons);*/

    Menu(ElementProperties &&properties, std::vector<std::optional<Menu>> submenus,
         std::vector<std::optional<Button>> buttons);

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
