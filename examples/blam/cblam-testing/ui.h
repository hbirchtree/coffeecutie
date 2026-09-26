#pragma once

#include <coffee/components/entity_container.h>

#include "data.h"
#include "selected_version.h"

struct UIEvent
{
    enum type_t
    {
        navigation,
    } type;
};

struct UINavigation
{
    static constexpr auto event_type = UIEvent::navigation;

    enum action_t
    {
        back,
        accept,

        up,
        down,
        left,
        right,
    } action{accept};
    libc_types::u32 seat_idx{};
};

using UIEventBus = comp_app::BasicEventBus<UIEvent>;

void alloc_ui_system(compo::EntityContainer& e);

void load_ui_items(
    compo::EntityContainer& e, MapChangedEvent<halo_version>& data);
