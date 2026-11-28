#pragma once

#include "ui/ui_game_setup.h"

#include <coffee/components/entity_container.h>
#include <coffee/components/subsystem.h>

#include <peripherals/typing/vectors/vector_types.h>

#include <array>
#include <set>
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
    f32                    over_for{0.f};   /*!< Seconds since it ended, as this peer saw */

    /*! Play carries on a moment past the end before the results show */
    static constexpr f32 results_delay = 3.f;
    bool shows_results() const
    {
        return phase == phase_t::over && over_for >= results_delay;
    }
    /*! Seconds a vehicle sits unused away from its spot before it is put
     *  back; below 0, never */
    f32 vehicle_respawn{default_vehicle_respawn};
    static constexpr f32 default_vehicle_respawn = 60.f;
    bool                   ai_enabled{true}; /*!< Scripts' ai_enable: actors think */
    bool                   ai_grenades{true}; /*!< Scripts' ai_grenades: actors throw them */
    bool                   ai_dialogue{true}; /*!< ai_dialogue_triggers: actors speak up in combat */
    std::set<libc_types::u64> preferred_targets; /*!< ai_prefer_target: picked over nearer ones */
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

    /*! Capture the flag: each team's flag (red 0, blue 1), and Oddball's
     *  ball (2). The authority keeps them; the rest is told whether each is
     *  taken and by whom */
    struct flag_t
    {
        typing::vector_types::Vecf3 base{};
        bool                        placed{false}; /*!< The map has a base for it */
        bool                        taken{false};  /*!< Gone from its base since an enemy took it */
        i32                         carrier{-1};   /*!< Player index holding it */
        f32                         idle{0.f};     /*!< Seconds lying away from its base */
        bool                        removed{false}; /*!< Out of play, e.g. the attackers' in single flag */
        libc_types::u64             object{0};     /*!< Where it lies, on the authority */
    };
    std::array<flag_t, 3> flags{};
    static constexpr size_t ball = 2; /*!< Oddball's, among the flags */
    i32                   juggernaut{-1}; /*!< Juggernaut: the player index who is it */
    bool                  timed{false};   /*!< Scores are seconds (King of the hill, Oddball) */
    bool                  no_tracker{false}; /*!< Nobody has a motion tracker */
    libc_types::f32       player_health{1.f}; /*!< Players' body vitality, times the coll's */
    /*! Who is camouflaged by the game: nobody, everyone, or whoever is "it" */
    enum class unseen_t : libc_types::u8
    {
        nobody,
        everyone,
        it,
    } unseen{unseen_t::nobody};

    bool unseen_by_game(u32 player_idx) const
    {
        return unseen == unseen_t::everyone ||
               (unseen == unseen_t::it && juggernaut >= 0 && juggernaut == static_cast<i32>(player_idx));
    }
    i16                   hill{-1};        /*!< King of the hill: the hill in play */
    /*! Who holds it: a player (with teams, any of the one team in it), -1
     *  for nobody, contested for more than one side */
    i16                   hill_holder{-1};
    static constexpr i16  contested = -2;
    i16                   capture_blocked{-1}; /*!< A carrier kept from scoring, for a moment */
    f32                   capture_blocked_for{0.f};
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

    /*! Infection's infected are its team 1 */
    static constexpr i16 infected_team = 1;
    bool infected(u32 player_idx) const
    {
        auto const* p = player(player_idx);
        return game_type == GameSetup::game_type_t::infection && p && p->team == infected_team;
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
        case t::infection:
            return "Infection";
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
