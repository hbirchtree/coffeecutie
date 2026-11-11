#pragma once

#include "hsc_builder.h"
#include "ui_game_setup.h"

/* Multiplayer rules as HaloScript, built in memory. The engine counts kills
 * and deaths, then wakes `on_unit_killed` with `event_killer` and
 * `event_victim` set (player indices, -1 for none); the rules give out
 * score and end the game through the functions below. They run where the
 * game is decided.
 *
 * Each game type also has an announcer program, run by every peer for its
 * own players from what everyone is told: teams, each player's kills and
 * deaths, and the game's phase. It wakes `on_local_kill` with
 * `event_player` set when one of them kills, and `on_game_over` */
namespace game_modes {

/* Past every map dialect's opcodes, so only the game mode's natives see them */
enum class mode_op : libc_types::u16
{
    score_add = 0x400,  /*!< (score_add <player> <short>) */
    player_score,       /*!< (player_score <player>) */
    leader_score,       /*!< (leader_score): the best player or team score */
    same_team,          /*!< (same_team <player> <player>) */
    game_over,          /*!< (game_over): the leader wins */
    respawn_delay_set,  /*!< (respawn_delay_set <real seconds>) */
    time_limit_set,     /*!< (time_limit_set <real seconds>), 0 = none */
    vehicle_respawn_set, /*!< (vehicle_respawn_set <real seconds>), below 0 = never */

    /* The announcer's, on each peer */
    announce,     /*!< (announce <short sound>): matg's multiplayer sound, played here */
    teams,        /*!< (teams): the game has teams */
    kills_within, /*!< (kills_within <player> <real seconds>): their kills that recent */
    kill_streak,  /*!< (kill_streak <player>): their kills since last dying */
};

/* matg's multiplayer sounds, by their place in the block */
enum class announcer : libc_types::i16
{
    game_over     = 1,
    double_kill   = 14,
    triple_kill   = 15,
    killtacular   = 16,
    running_riot  = 17,
    killing_spree = 18,
    slayer        = 21,
    team_slayer   = 35,
};

/* Kills this close together are one multi-kill; my pick */
constexpr libc_types::f32 multi_kill_seconds = 4.f;

template<typename BC>
constexpr BC op(mode_op o)
{
    return static_cast<BC>(static_cast<libc_types::u16>(o));
}

/* Kills score a point; deaths by your own hand or the world's cost one,
 * as does killing a teammate. First to the limit wins */
template<typename BC>
hsc_builder::program_t<BC> slayer(libc_types::i16 score_limit, libc_types::f32 time_limit)
{
    using hsc_builder::script_type_t;
    using hsc_builder::type_t;
    hsc_builder::builder<BC> b;
    auto const killer = [&b] { return b.global("event_killer", type_t::short_); };
    auto const victim = [&b] { return b.global("event_victim", type_t::short_); };

    b.declare_global("score_limit", type_t::short_, b.short_(score_limit));
    b.declare_global("event_killer", type_t::short_, b.short_(-1));
    b.declare_global("event_victim", type_t::short_, b.short_(-1));

    b.script(
        "setup",
        script_type_t::startup,
        b.begin({
            b.call(op<BC>(mode_op::respawn_delay_set), type_t::void_, {b.real(3.f)}),
            b.call(op<BC>(mode_op::time_limit_set), type_t::void_, {b.real(time_limit)}),
        }));

    auto const no_killer =
        b.call(BC::equal, type_t::bool_, {killer(), b.short_(-1)});
    auto const suicide = b.call(BC::equal, type_t::bool_, {killer(), victim()});
    auto const betrayal = b.call(op<BC>(mode_op::same_team), type_t::bool_, {killer(), victim()});
    auto const penalty =
        b.call(BC::or_, type_t::bool_, {no_killer, suicide});
    b.script(
        "on_unit_killed",
        script_type_t::dormant,
        b.begin({
            b.if_(
                penalty,
                b.call(op<BC>(mode_op::score_add), type_t::void_, {victim(), b.short_(-1)}),
                b.if_(
                    betrayal,
                    b.call(op<BC>(mode_op::score_add), type_t::void_, {killer(), b.short_(-1)}),
                    b.call(op<BC>(mode_op::score_add), type_t::void_, {killer(), b.short_(1)}))),
            b.if_(
                b.call(
                    BC::gequals,
                    type_t::bool_,
                    {b.call(op<BC>(mode_op::leader_score), type_t::short_, {}),
                     b.global("score_limit", type_t::short_)}),
                b.call(op<BC>(mode_op::game_over), type_t::void_, {})),
        }));
    return std::move(b).build();
}

/* Slayer's announcer: the game type as it starts or is joined, multi-kills
 * and sprees to the player who made them, and the game's end */
template<typename BC>
hsc_builder::program_t<BC> slayer_announcer()
{
    using hsc_builder::script_type_t;
    using hsc_builder::type_t;
    using i16 = libc_types::i16;
    hsc_builder::builder<BC> b;
    auto const player   = [&b] { return b.global("event_player", type_t::short_); };
    auto const announce = [&b](announcer sound) {
        return b.call(op<BC>(mode_op::announce), type_t::void_, {b.short_(static_cast<i16>(sound))});
    };
    auto const recent = [&](i16 n) {
        return b.call(
            BC::equal,
            type_t::bool_,
            {b.call(op<BC>(mode_op::kills_within), type_t::short_, {player(), b.real(multi_kill_seconds)}),
             b.short_(n)});
    };
    auto const streak = [&](i16 n) {
        return b.call(
            BC::equal, type_t::bool_, {b.call(op<BC>(mode_op::kill_streak), type_t::short_, {player()}), b.short_(n)});
    };

    b.declare_global("event_player", type_t::short_, b.short_(-1));
    b.script(
        "announce_game",
        script_type_t::startup,
        b.if_(
            b.call(op<BC>(mode_op::teams), type_t::bool_, {}),
            announce(announcer::team_slayer),
            announce(announcer::slayer)));
    b.script(
        "on_local_kill",
        script_type_t::dormant,
        b.begin({
            b.if_(
                recent(2),
                announce(announcer::double_kill),
                b.if_(
                    recent(3),
                    announce(announcer::triple_kill),
                    b.if_(recent(4), announce(announcer::killtacular)))),
            b.if_(
                streak(5),
                announce(announcer::killing_spree),
                b.if_(streak(10), announce(announcer::running_riot))),
        }));
    b.script("on_game_over", script_type_t::dormant, announce(announcer::game_over));
    return std::move(b).build();
}

/* The announcer for a game type */
template<typename BC>
hsc_builder::program_t<BC> announcer_for(GameSetup::game_type_t type)
{
    switch(type)
    {
    default:
        return slayer_announcer<BC>();
    }
}

/* The rules for a game type; ones not written yet play as Slayer */
template<typename BC>
hsc_builder::program_t<BC> rules_for(GameSetup::game_type_t type)
{
    switch(type)
    {
    default:
        return slayer<BC>(25, 0.f);
    }
}

} // namespace game_modes
