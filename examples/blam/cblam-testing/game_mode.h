#pragma once

#include "ui_game_setup.h"

#include <coffee/components/entity_container.h>
#include <coffee/components/subsystem.h>

#include <array>
#include <string_view>
#include <vector>

/*! The game being played: what kind, how far along, and the score. The
 *  authority keeps it; everyone else is told */
struct GameState : compo::SubsystemBase
{
    using type = GameState;
    using f32  = libc_types::f32;
    using i16  = libc_types::i16;
    using i32  = libc_types::i32;
    using u16  = libc_types::u16;
    using u32  = libc_types::u32;

    enum class kind_t : libc_types::u8
    {
        none, /*!< The menu */
        campaign,
        multiplayer,
    };

    enum class phase_t : libc_types::u8
    {
        playing,
        over,
    };

    struct player_t
    {
        u32 player_idx{0};
        i16 team{-1}; /*!< -1 without teams */
        i16 score{0};
        i16 kills{0};
        i16 deaths{0};
    };

    kind_t                 kind{kind_t::none};
    phase_t                phase{phase_t::playing};
    GameSetup::game_type_t game_type{GameSetup::game_type_t::slayer};
    bool                   teams{false};
    u16                    difficulty{1}; /*!< easy, normal, heroic, legendary */
    std::vector<player_t>  players;
    std::array<i16, 2>     team_scores{};
    i32                    winner{-1}; /*!< A player index, or a team with teams */
    f32                    time_left{-1.f}; /*!< Seconds; below 0 without a limit */
    /*! Seconds a vehicle sits unused away from its spot before it is put
     *  back; below 0, never */
    f32 vehicle_respawn{default_vehicle_respawn};
    static constexpr f32 default_vehicle_respawn = 60.f;
    u32                    saves{0};   /*!< Checkpoints taken */
    u32                    reverts{0}; /*!< Times sent back to one */
    /*! Campaign, on the authority: nobody hurt for a while, so the fallen
     *  may come back and a checkpoint may be taken */
    bool quiet{true};
    bool saving{false}; /*!< A checkpoint waits for quiet */

    player_t* player(u32 player_idx)
    {
        for(auto& p : players)
            if(p.player_idx == player_idx)
                return &p;
        return nullptr;
    }

    player_t const* player(u32 player_idx) const
    {
        return const_cast<GameState*>(this)->player(player_idx);
    }

    static std::string_view title(GameSetup::game_type_t type, bool teams)
    {
        using t = GameSetup::game_type_t;
        switch(type)
        {
        case t::ctf:
            return "Capture the Flag";
        case t::koth:
            return "King of the Hill";
        case t::oddball:
            return "Oddball";
        case t::race:
            return "Race";
        default:
            return teams ? "Team Slayer" : "Slayer";
        }
    }
};

/*! The game state, with a controller for campaign and one for multiplayer */
void alloc_game_mode(compo::EntityContainer& e);

void alloc_campaign_controller(compo::EntityContainer& e);
void alloc_multiplayer_controller(compo::EntityContainer& e);
/*! Multiplayer items and vehicles of the map, put back on their timers */
void alloc_object_respawn(compo::EntityContainer& e);
