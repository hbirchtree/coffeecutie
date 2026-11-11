#pragma once

#include "ui_game_setup.h"

#include <coffee/components/entity_container.h>
#include <coffee/components/subsystem.h>

#include <peripherals/typing/vectors/vector_types.h>

#include <array>
#include <string>
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
    bool                   ai_enabled{true}; /*!< Scripts' ai_enable: actors think */
    u32                    saves{0};   /*!< Checkpoints taken */
    u32                    reverts{0}; /*!< Times sent back to one */
    /*! Sent back since the last checkpoint: game_reverted, which a skipped
     *  cutscene is told apart by */
    bool reverted_since_save{false};
    /*! Campaign, on the authority: nobody hurt for a while, so the fallen
     *  may come back and a checkpoint may be taken */
    bool quiet{true};
    bool saving{false}; /*!< A checkpoint waits for quiet */

    /*! A HUD marker over a spot or an object, drawn in one of hudg's
     *  waypoint arrows for a team or one player */
    struct nav_point_t
    {
        std::string                      arrow;  /*!< The waypoint arrow's name */
        std::string                      flag;   /*!< The cutscene flag marked; empty for an object */
        typing::vector_types::Vecf3      position{};
        libc_types::u64                  object{0};
        i32                              unit{-1}; /*!< A player's body, by player index */
        libc_types::u32                  net{0};   /*!< An object not yet here, by net id */
        f32                              offset{0.f}; /*!< wu above it */
        i16                              team{-1};    /*!< unit team_t; -1 for one player */
        i32                              player{-1};  /*!< player index, when not a team's */
        i16                              mp_team{-1}; /*!< Shown only to this multiplayer team */

        bool operator==(nav_point_t const&) const = default;
    };
    std::vector<nav_point_t> nav_points;

    /*! Capture the flag: each team's flag (red 0, blue 1). The authority
     *  keeps them; the rest is told whether each is taken and by whom */
    struct flag_t
    {
        typing::vector_types::Vecf3 base{};
        bool                        placed{false}; /*!< The map has a base for it */
        bool                        taken{false};  /*!< Gone from its base since an enemy took it */
        i32                         carrier{-1};   /*!< Player index holding it */
        f32                         idle{0.f};     /*!< Seconds lying away from its base */
        libc_types::u64             object{0};     /*!< Where it lies, on the authority */
    };
    std::array<flag_t, 2> flags{};
    f32                   flag_reach{.8f}; /*!< wu, set by the rules */

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
