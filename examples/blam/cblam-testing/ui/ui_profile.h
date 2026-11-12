#pragma once

#include <coffee/components/entity_container.h>

#include <array>
#include <string>
#include <vector>

/* The local player's profile, as the shell UI edits it. Values are indices
 * into the spinners' own string lists until real settings exist. */
struct PlayerProfile : compo::SubsystemBase
{
    using type = PlayerProfile;

    std::u16string name{u"Player"};
    std::u16string game_setting_name;

    struct controls_t
    {
        libc_types::u16 invert_look{0};   /* YES */
        libc_types::u16 sensitivity{2};   /* 3 */
        libc_types::u16 vibration{0};     /* YES */
        libc_types::u16 invert_flight{1}; /* NO */
        libc_types::u16 auto_center{1};   /* NO */
    } controls;
};

/* Controllers that joined a split screen lobby, indexed by controller; the
 * next game map seats them in controller order */
struct LocalLobby : compo::SubsystemBase
{
    using type = LocalLobby;

    std::array<bool, 4> joined{};
    std::array<bool, 4> profile_chosen{};

    std::vector<libc_types::u32> seated_controllers() const
    {
        std::vector<libc_types::u32> out;
        for(libc_types::u32 i = 0; i < joined.size(); ++i)
            if(joined[i])
                out.push_back(i);
        return out;
    }
};

/* Registers PlayerProfile, LocalLobby and their UIDataSource hooks; after
 * alloc_ui_system */
void alloc_profile_provider(compo::EntityContainer& e);
