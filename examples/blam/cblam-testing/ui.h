#pragma once

#include <coffee/components/entity_container.h>

#include "data.h"
#include "selected_version.h"

struct UIEvent
{
    enum type_t
    {
        navigation,
        menu_leave,
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

using UIEventBus = comp_app::BasicEventBus<UIEvent>;

void alloc_ui_system(compo::EntityContainer& e);

void load_ui_items(
    compo::EntityContainer& e, MapChangedEvent<halo_version>& data);
