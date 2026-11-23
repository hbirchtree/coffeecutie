#pragma once

#include <coffee/components/entity_container.h>

#include <peripherals/typing/vectors/vector_types.h>

#include <array>
#include <string>
#include <vector>

/* Players' profiles, as the shell UI edits them and profiles.json keeps
 * them: each with a colour and controls. Control values are indices into the
 * spinners' own string lists */
struct PlayerProfile : compo::SubsystemBase
{
    using type = PlayerProfile;

    struct controls_t
    {
        libc_types::u16 invert_look{0};   /* YES */
        libc_types::u16 sensitivity{2};   /* 3 */
        libc_types::u16 vibration{0};     /* YES */
        libc_types::u16 invert_flight{1}; /* NO */
        libc_types::u16 auto_center{1};   /* NO */
    };

    struct profile_t
    {
        std::u16string  name{u"Player"};
        libc_types::u16 color{0}; /*!< Into colors */
        controls_t      controls;
    };

    std::vector<profile_t> profiles{profile_t{}};
    libc_types::u16        editing{0};  /*!< The one the edit screens show */
    libc_types::u16        selected{0}; /*!< Focused in a profile list */
    std::u16string         game_setting_name;

    /* CE's armour colours, in its picker's order */
    struct color_t
    {
        char16_t const*                 name;
        typing::vector_types::Vecf3 rgb;
    };
    static std::array<color_t, 18> const colors;

    profile_t& edited()
    {
        if(editing >= profiles.size())
            editing = 0;
        return profiles.at(editing);
    }

    profile_t const& at(libc_types::i32 index) const
    {
        return profiles.at(index >= 0 && static_cast<size_t>(index) < profiles.size() ? static_cast<size_t>(index) : 0);
    }

    /* profiles.json, beside config.json */
    void load();
    void save() const;
};

/* Controllers that joined a split screen lobby, indexed by controller; the
 * next game map seats them in controller order */
struct LocalLobby : compo::SubsystemBase
{
    using type = LocalLobby;

    std::array<bool, 4> joined{};
    std::array<bool, 4> profile_chosen{};
    /*! Each controller's profile, by index; the first profile until chosen */
    std::array<libc_types::i32, 4> profile{};
    /*! Each controller's team in a team game: red 0, blue 1 */
    std::array<libc_types::i16, 4> team{0, 1, 0, 1};

    /*! Whether the joined controllers make up both teams */
    bool both_teams() const
    {
        std::array<bool, 2> seen{};
        for(libc_types::u32 i = 0; i < joined.size(); ++i)
            if(joined[i])
                seen[static_cast<size_t>(team[i] & 1)] = true;
        return seen[0] && seen[1];
    }

    /*! The profile a controller plays with */
    libc_types::i32 profile_of(libc_types::u32 controller) const
    {
        return controller < profile.size() ? profile[controller] : 0;
    }

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
