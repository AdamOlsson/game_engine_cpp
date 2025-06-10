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

    std::vector<std::optional<Menu>> m_submenus;
    std::vector<std::optional<Button>> m_buttons;

    void setup_navigation_callback();
    void update_vectors();

  public:
    std::vector<ElementProperties *> create_properties_vector();
    std::vector<Button *> create_button_vector();

    std::vector<Button *> button_vector;
    std::vector<ElementProperties *> properties_vector;

    /**
     * @brief Creates a menu which serves as the top level menu.
     *
     * Default constructor creates an empty menu that can serve as the root of a menu
     * hierarchy. This menu will not have a back button since it's the top level.
     *
     * @since C++20
     */
    Menu() = default;

    Menu(Menu &&other) = default;
    Menu(const Menu &other) = default;

    Menu &operator=(Menu &&other) = default;
    Menu &operator=(const Menu &other) = default;

    /**
     * @brief Creates a menu which serves as a submenu.
     *
     * The menu can contain buttons or additional submenus. The menu items handle
     * logic for transitioning up and down menus. When the main button is clicked,
     * the UI will navigate to this submenu. When the back button is clicked,
     * the UI will return to the parent menu.
     *
     * @param button The button which, once pressed, will display the submenu
     * @param back_button The button that will navigate back to the parent menu
     *
     * @note The button callbacks will be automatically wrapped to include navigation
     * logic
     * @see Button
     * @since C++20
     */
    Menu(Button &button, Button &back_button);

    /**
     * @brief Creates a menu which serves as a submenu (move version).
     *
     * Move constructor version that takes ownership of the provided buttons.
     *
     * @param button The button which, once pressed, will display the submenu
     * @param back_button The button that will navigate back to the parent menu
     *
     * @see Menu(Button &button, Button &back_button)
     * @since C++20
     */
    Menu(Button &&button, Button &&back_button);

    /**
     * @brief Adds a button to this menu (move version).
     *
     * Adds a regular button that performs an action when clicked but doesn't
     * navigate to another menu. Uses move semantics for efficiency.
     *
     * @param button The button to add to this menu
     * @return Reference to this menu for method chaining
     *
     * @example
     * @code
     * menu.add_button(Button("Save Game").set_on_click(save_callback))
     *     .add_button(Button("Load Game").set_on_click(load_callback));
     * @endcode
     */
    Menu &add_button(Button &&button);

    /**
     * @brief Adds a button to this menu (copy version).
     *
     * Adds a regular button that performs an action when clicked but doesn't
     * navigate to another menu. This version creates a copy of the button.
     *
     * @param button The button to add to this menu
     * @return Reference to this menu for method chaining
     *
     * @note Prefer the move version (Button&&) for better performance
     */
    Menu &add_button(Button &button);

    /**
     * @brief Adds a submenu to this menu (move version).
     *
     * Adds a submenu that can be navigated to when its associated button is clicked.
     * The submenu must be constructed with both a main button and a back button.
     * Uses move semantics for efficiency.
     *
     * @param menu The submenu to add to this menu
     * @return Reference to this menu for method chaining
     *
     * @example
     * @code
     * menu.add_submenu(Menu(Button("Settings"), Button("Back"))
     *                  .add_button(Button("Audio"))
     *                  .add_button(Button("Video")));
     * @endcode
     */
    Menu &add_submenu(Menu &&menu);

    /**
     * @brief Adds a submenu to this menu (copy version).
     *
     * Adds a submenu that can be navigated to when its associated button is clicked.
     * This version creates a copy of the submenu.
     *
     * @param menu The submenu to add to this menu
     * @return Reference to this menu for method chaining
     *
     * @note Prefer the move version (Menu&&) for better performance
     */
    Menu &add_submenu(Menu &menu);

    /**
     * @brief Links this menu and all its submenus to the UI system.
     *
     * This method establishes the connection between the menu hierarchy and the UI
     * after all menu construction, moving, and copying operations are complete.
     * It must be called before the menu can be used for navigation.
     *
     * The method performs the following operations:
     * 1. Stores a pointer to the UI instance for navigation callbacks
     * 2. Sets up navigation callbacks for menu buttons and back buttons
     * 3. Recursively links all submenus to the same UI instance
     * 4. Updates internal vectors for efficient access to buttons and properties
     *
     * @param ui Pointer to the UI instance that will manage this menu system.
     *           Must not be nullptr and must remain valid for the lifetime of the menu.
     *
     * @note This method is automatically called by the UI constructor and should not
     *       be called manually unless you're managing menus outside of the standard
     *       UI framework.
     *
     * @note The linking process is necessary due to the design decision to avoid
     *       unique_ptr usage in the public API. Since menus undergo significant
     *       moving and copying during construction, reliable pointers can only be
     *       established after all operations are complete.
     *
     * @warning Attempting to use menu navigation before calling link() will result
     *          in warning messages and non-functional navigation.
     *
     * @see setup_navigation_callback() for details on callback setup
     * @see update_vectors() for details on internal vector management
     */
    void link(UI *ui);
};

} // namespace ui
