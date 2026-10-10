#pragma once

#include <coffee/components/entity_container.h>

#include "data.h"
#include "selected_version.h"

#include <string>

/* Text over the 3D view outside any Halo menu, like the forge menu. The UI
 * renderer draws it in window pixels, y down, and clears it every frame. */
struct ScreenText : compo::SubsystemBase
{
    using type = ScreenText;

    struct line_t
    {
        std::u16string text;
        Vecf2          baseline; /*!< Where the baseline starts */
        Vecf4          color{1.f};
        f32            max_width{0.f}; /*!< Cut off past this; 0 = no limit */
    };

    std::vector<line_t> lines;
};

struct UIEvent
{
    enum type_t
    {
        navigation,
        menu_leave,
        function_done,
        open_widget,
    } type;
};

struct UINavigation
{
    static constexpr auto event_type = UIEvent::navigation;

    enum action_t
    {
        back,
        option,   // Y button
        option_2, // X button
        accept,
        open,  /*!< show the seat's pause menu */
        close, /*!< hide it, e.g. when the player toggles back to game */

        up,
        down,
        left,
        right,
    } action{accept};
    libc_types::u32 seat_idx{};
};

/* The UI closed a menu on its own (B, a resume button); whoever sent
 * UINavigation::close is not told */
struct UIMenuLeave
{
    static constexpr auto event_type = UIEvent::menu_leave;

    libc_types::u32 seat_idx{};
};

/* Finishes a provider function that returned ui_result_t::pending */
struct UIFunctionDone
{
    static constexpr auto event_type = UIEvent::function_done;

    libc_types::u64 token{};
    bool            ok{true};
};

/* Opens a widget by tag path on the seat's screen, e.g. an error dialog */
struct UIOpenWidget
{
    static constexpr auto event_type = UIEvent::open_widget;

    libc_types::u32 seat_idx{};
    std::string     widget;
};

using UIEventBus = comp_app::BasicEventBus<UIEvent>;

void alloc_ui_system(compo::EntityContainer& e);

void load_ui_items(
    compo::EntityContainer& e, MapChangedEvent<halo_version>& data);
