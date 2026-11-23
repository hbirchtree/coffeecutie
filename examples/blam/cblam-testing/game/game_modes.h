#pragma once

#include "scripting/hsc_builder.h"
#include "ui/ui_game_setup.h"

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

    /* Capture the flag, by team (red 0, blue 1) */
    flag_give,        /*!< (flag_give <team> <player>): into their hands */
    flag_reset,       /*!< (flag_reset <team>): back to its base */
    flag_taken,       /*!< (flag_taken <team>): gone from its base since an enemy took it */
    flag_carrier,     /*!< (flag_carrier <team>): the player holding it, -1 for none */
    flag_idle,        /*!< (flag_idle <team>): seconds it has lain away from its base */
    team_of,          /*!< (team_of <player>) */
    player_near_base, /*!< (player_near_base <player> <team> <real wu>): within that of the team's base */
    flag_reach_set,   /*!< (flag_reach_set <real wu>): how near a player touches a flag */

    /* The announcer's, on each peer */
    announce,     /*!< (announce <short sound>): matg's multiplayer sound, played here */
    teams,        /*!< (teams): the game has teams */
    kills_within, /*!< (kills_within <player> <real seconds>): their kills that recent */
    kill_streak,  /*!< (kill_streak <player>): their kills since last dying */
    team_score,   /*!< (team_score <team>) */

    /* Juggernaut, on the authority */
    juggernaut_set, /*!< (juggernaut_set <player>): marks who is it, -1 for nobody */

    /* King of the hill; the announcer has the last two too */
    hill_count,  /*!< (hill_count): the map's hills */
    hill_set,    /*!< (hill_set <short>): the hill in play */
    hill_holder, /*!< (hill_holder): a player holding it alone (or their team), -1 nobody, -2 contested */
    hill_index,  /*!< (hill_index): the hill in play, -1 for none */

    /* Race, on the authority */
    race_count,   /*!< (race_count): the track's checkpoints */
    race_next,    /*!< (race_next <player>): the checkpoint they go for */
    race_advance, /*!< (race_advance <player>): on to the next, round to the first after the last */
};

/* matg's multiplayer sounds, by their place in the block */
enum class announcer : libc_types::i16
{
    play_ball     = 0,
    game_over     = 1,
    double_kill   = 14,
    triple_kill   = 15,
    killtacular   = 16,
    running_riot  = 17,
    killing_spree = 18,
    blue_team_has_the_flag  = 8,
    blue_team_flag_returned = 9,
    blue_team_score         = 10,
    red_team_has_the_flag   = 11,
    red_team_flag_returned  = 12,
    red_team_score          = 13,
    oddball                 = 19,
    slayer                  = 21,
    capture_the_flag        = 22,
    team_oddball            = 33,
    team_slayer             = 35,
    hill_move               = 30,
    team_king_of_the_hill   = 32,
    king_of_the_hill        = 36,
    race                    = 20,
    team_race               = 34,
    hill_contested          = 39,
    hill_controlled         = 40,
};

/* Oddball's ball, among capture the flag's flags (GameState::ball) */
constexpr libc_types::i16 ball_slot = 2;

/* How often the takers of a flag hear they still have it (20 s) */
constexpr libc_types::i16 flag_reminder_ticks = 600;

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
    b.declare_global("time_limit", type_t::real_, b.real(time_limit));
    b.declare_global("respawn_delay", type_t::real_, b.real(3.f));
    b.declare_global("event_killer", type_t::short_, b.short_(-1));
    b.declare_global("event_victim", type_t::short_, b.short_(-1));

    b.script(
        "setup",
        script_type_t::startup,
        b.begin({
            b.call(op<BC>(mode_op::respawn_delay_set), type_t::void_, {b.global("respawn_delay", type_t::real_)}),
            b.call(op<BC>(mode_op::time_limit_set), type_t::void_, {b.global("time_limit", type_t::real_)}),
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

/* As the decomp's capture the flag plays it: an enemy touching a flag takes
 * it; a teammate touching one that was taken sends it back unless the
 * variant says it must reset, one at its base stays. A carrier at their own
 * base scores, if the variant asks it only while their own flag is home. A
 * flag lying untouched goes back on its own (60 s; the decomp has 17).
 * Kills do not score */
template<typename BC>
hsc_builder::program_t<BC> ctf(GameSetup const& variant)
{
    using hsc_builder::script_type_t;
    using hsc_builder::type_t;
    using i16 = libc_types::i16;
    hsc_builder::builder<BC> b;
    auto const player  = [&b] { return b.global("event_player", type_t::short_); };
    auto const team    = [&b] { return b.global("event_team", type_t::short_); };
    auto const taken   = [&b](auto t) { return b.call(op<BC>(mode_op::flag_taken), type_t::bool_, {t}); };
    auto const carrier = [&b](auto t) { return b.call(op<BC>(mode_op::flag_carrier), type_t::short_, {t}); };
    auto const reset   = [&b](auto t) { return b.call(op<BC>(mode_op::flag_reset), type_t::void_, {t}); };

    b.declare_global("score_limit", type_t::short_, b.short_(3));
    b.declare_global("flag_at_home", type_t::bool_, b.boolean(variant.ctf_flag_at_home));
    b.declare_global("flag_must_reset", type_t::bool_, b.boolean(variant.ctf_flag_must_reset));
    b.declare_global("flag_return_time", type_t::real_, b.real(variant.ctf_flag_return_time));
    b.declare_global("capture_reach", type_t::real_, b.real(1.f));  /* wu from the base */
    b.declare_global("flag_reach", type_t::real_, b.real(.8f));     /* wu a player touches a flag at */
    b.declare_global("respawn_delay", type_t::real_, b.real(10.f));
    b.declare_global("event_killer", type_t::short_, b.short_(-1));
    b.declare_global("event_victim", type_t::short_, b.short_(-1));
    b.declare_global("event_player", type_t::short_, b.short_(-1));
    b.declare_global("event_team", type_t::short_, b.short_(-1));

    b.script(
        "setup",
        script_type_t::startup,
        b.begin({
            b.call(op<BC>(mode_op::respawn_delay_set), type_t::void_, {b.global("respawn_delay", type_t::real_)}),
            b.call(op<BC>(mode_op::time_limit_set), type_t::void_, {b.real(0.f)}),
            b.call(op<BC>(mode_op::flag_reach_set), type_t::void_, {b.global("flag_reach", type_t::real_)}),
        }));
    b.script(
        "on_flag_touched",
        script_type_t::dormant,
        b.if_(
            b.call(
                BC::equal,
                type_t::bool_,
                {b.call(op<BC>(mode_op::team_of), type_t::short_, {player()}), team()}),
            b.if_(
                b.call(
                    BC::and_,
                    type_t::bool_,
                    {taken(team()), b.call(BC::not_, type_t::bool_, {b.global("flag_must_reset", type_t::bool_)})}),
                reset(team())),
            b.if_(
                b.call(BC::less, type_t::bool_, {carrier(team()), b.short_(0)}),
                b.call(op<BC>(mode_op::flag_give), type_t::void_, {team(), player()}))));
    for(i16 own = 0; own < 2; own++)
    {
        i16 const enemy = 1 - own;
        b.script(
            own == 0 ? "red_capture" : "blue_capture",
            script_type_t::continuous,
            b.if_(
                b.call(
                    BC::and_,
                    type_t::bool_,
                    {b.call(BC::gequals, type_t::bool_, {carrier(b.short_(enemy)), b.short_(0)}),
                     b.call(
                         op<BC>(mode_op::player_near_base),
                         type_t::bool_,
                         {carrier(b.short_(enemy)), b.short_(own), b.global("capture_reach", type_t::real_)}),
                     b.call(
                         BC::or_,
                         type_t::bool_,
                         {b.call(BC::not_, type_t::bool_, {b.global("flag_at_home", type_t::bool_)}),
                          b.call(BC::not_, type_t::bool_, {taken(b.short_(own))})})}),
                b.begin({
                    b.call(op<BC>(mode_op::score_add), type_t::void_, {carrier(b.short_(enemy)), b.short_(1)}),
                    reset(b.short_(enemy)),
                    b.if_(
                        b.call(
                            BC::gequals,
                            type_t::bool_,
                            {b.call(op<BC>(mode_op::leader_score), type_t::short_, {}),
                             b.global("score_limit", type_t::short_)}),
                        b.call(op<BC>(mode_op::game_over), type_t::void_, {})),
                })));
        b.script(
            own == 0 ? "red_return" : "blue_return",
            script_type_t::continuous,
            b.if_(
                b.call(
                    BC::and_,
                    type_t::bool_,
                    {taken(b.short_(own)),
                     b.call(BC::less, type_t::bool_, {carrier(b.short_(own)), b.short_(0)}),
                     b.call(
                         BC::greater,
                         type_t::bool_,
                         {b.call(op<BC>(mode_op::flag_idle), type_t::real_, {b.short_(own)}),
                          b.global("flag_return_time", type_t::real_)})}),
                reset(b.short_(own))));
    }
    return std::move(b).build();
}

/* Oddball, as CE plays it: the ball starts at one of its spawns; whoever
 * touches it on the ground takes it and scores a point a second carrying it.
 * Dropped, it lies until picked up or gone back (30 s, my pick). First to
 * the limit wins. Ball type 2 is Juggernaut instead: no ball, the first to
 * kill is it, only it scores kills, and whoever kills it takes over */
template<typename BC>
hsc_builder::program_t<BC> oddball()
{
    using hsc_builder::script_type_t;
    using hsc_builder::type_t;
    hsc_builder::builder<BC> b;
    auto const short_g = [&b](char const* name) { return b.global(name, type_t::short_); };
    auto const ball    = [&b] { return b.short_(ball_slot); };
    auto const carrier = [&] { return b.call(op<BC>(mode_op::flag_carrier), type_t::short_, {ball()}); };
    auto const juggernaut_game = [&] { return b.call(BC::equal, type_t::bool_, {short_g("ball_type"), b.short_(2)}); };
    auto const check_limit = [&](char const* limit) {
        return b.if_(
            b.call(
                BC::gequals,
                type_t::bool_,
                {b.call(op<BC>(mode_op::leader_score), type_t::short_, {}), short_g(limit)}),
            b.call(op<BC>(mode_op::game_over), type_t::void_, {}));
    };
    auto const crown = [&](auto who) {
        return b.begin({
            b.set("juggernaut", type_t::short_, who),
            b.call(op<BC>(mode_op::juggernaut_set), type_t::void_, {short_g("juggernaut")}),
        });
    };

    b.declare_global("score_limit", type_t::short_, b.short_(120)); /* seconds carried */
    b.declare_global("kill_limit", type_t::short_, b.short_(15));   /* Juggernaut's */
    b.declare_global("ball_type", type_t::short_, b.short_(0));
    b.declare_global("ball_return_time", type_t::real_, b.real(30.f));
    b.declare_global("flag_reach", type_t::real_, b.real(.8f));
    b.declare_global("respawn_delay", type_t::real_, b.real(5.f));
    b.declare_global("juggernaut", type_t::short_, b.short_(-1));
    b.declare_global("event_killer", type_t::short_, b.short_(-1));
    b.declare_global("event_victim", type_t::short_, b.short_(-1));
    b.declare_global("event_player", type_t::short_, b.short_(-1));
    b.declare_global("event_team", type_t::short_, b.short_(-1));

    b.script(
        "setup",
        script_type_t::startup,
        b.begin({
            b.call(op<BC>(mode_op::respawn_delay_set), type_t::void_, {b.global("respawn_delay", type_t::real_)}),
            b.call(op<BC>(mode_op::time_limit_set), type_t::void_, {b.real(0.f)}),
            b.call(op<BC>(mode_op::flag_reach_set), type_t::void_, {b.global("flag_reach", type_t::real_)}),
            b.if_(
                b.call(BC::not_, type_t::bool_, {juggernaut_game()}),
                b.call(op<BC>(mode_op::flag_reset), type_t::void_, {ball()})),
        }));
    b.script(
        "on_flag_touched",
        script_type_t::dormant,
        b.if_(
            b.call(
                BC::and_,
                type_t::bool_,
                {b.call(BC::equal, type_t::bool_, {short_g("event_team"), ball()}),
                 b.call(BC::less, type_t::bool_, {carrier(), b.short_(0)})}),
            b.call(op<BC>(mode_op::flag_give), type_t::void_, {ball(), short_g("event_player")})));
    b.script(
        "carried",
        script_type_t::continuous,
        b.begin({
            b.call(BC::sleep, type_t::void_, {b.short_(30)}),
            b.if_(
                b.call(BC::gequals, type_t::bool_, {carrier(), b.short_(0)}),
                b.begin({
                    b.call(op<BC>(mode_op::score_add), type_t::void_, {carrier(), b.short_(1)}),
                    check_limit("score_limit"),
                })),
        }));
    b.script(
        "ball_return",
        script_type_t::continuous,
        b.if_(
            b.call(
                BC::and_,
                type_t::bool_,
                {b.call(op<BC>(mode_op::flag_taken), type_t::bool_, {ball()}),
                 b.call(BC::less, type_t::bool_, {carrier(), b.short_(0)}),
                 b.call(
                     BC::greater,
                     type_t::bool_,
                     {b.call(op<BC>(mode_op::flag_idle), type_t::real_, {ball()}),
                      b.global("ball_return_time", type_t::real_)})}),
            b.call(op<BC>(mode_op::flag_reset), type_t::void_, {ball()})));

    auto const killer   = [&] { return short_g("event_killer"); };
    auto const victim   = [&] { return short_g("event_victim"); };
    auto const is       = [&](auto a, auto c) { return b.call(BC::equal, type_t::bool_, {a, c}); };
    auto const no_kill  = b.call(BC::or_, type_t::bool_, {is(killer(), b.short_(-1)), is(killer(), victim())});
    b.script(
        "on_unit_killed",
        script_type_t::dormant,
        b.if_(
            juggernaut_game(),
            b.begin({
                b.if_(
                    no_kill,
                    /* It fell by its own hand or the world's: the next kill crowns */
                    b.if_(is(victim(), short_g("juggernaut")), crown(b.short_(-1))),
                    b.if_(
                        b.call(
                            BC::or_,
                            type_t::bool_,
                            {b.call(BC::less, type_t::bool_, {short_g("juggernaut"), b.short_(0)}),
                             is(victim(), short_g("juggernaut"))}),
                        b.begin({
                            crown(killer()),
                            b.call(op<BC>(mode_op::score_add), type_t::void_, {killer(), b.short_(1)}),
                        }),
                        b.if_(
                            is(killer(), short_g("juggernaut")),
                            b.call(op<BC>(mode_op::score_add), type_t::void_, {killer(), b.short_(1)})))),
                check_limit("kill_limit"),
            })));
    return std::move(b).build();
}

/* King of the hill: whoever holds the hill alone, or whose team alone is
 * in it, scores a point a second there. With a moving hill it goes on to
 * the next every so often (60 s, my pick). First to the limit wins */
template<typename BC>
hsc_builder::program_t<BC> koth()
{
    using hsc_builder::script_type_t;
    using hsc_builder::type_t;
    hsc_builder::builder<BC> b;
    auto const holder = [&b] { return b.call(op<BC>(mode_op::hill_holder), type_t::short_, {}); };

    b.declare_global("score_limit", type_t::short_, b.short_(120)); /* seconds held */
    b.declare_global("moving_hill", type_t::bool_, b.boolean(false));
    b.declare_global("hill_move_time", type_t::real_, b.real(60.f));
    b.declare_global("respawn_delay", type_t::real_, b.real(3.f));
    b.declare_global("event_killer", type_t::short_, b.short_(-1));
    b.declare_global("event_victim", type_t::short_, b.short_(-1));

    b.script(
        "setup",
        script_type_t::startup,
        b.begin({
            b.call(op<BC>(mode_op::respawn_delay_set), type_t::void_, {b.global("respawn_delay", type_t::real_)}),
            b.call(op<BC>(mode_op::time_limit_set), type_t::void_, {b.real(0.f)}),
            b.call(op<BC>(mode_op::hill_set), type_t::void_, {b.short_(0)}),
        }));
    b.script(
        "held",
        script_type_t::continuous,
        b.begin({
            b.call(BC::sleep, type_t::void_, {b.short_(30)}),
            b.if_(
                b.call(BC::gequals, type_t::bool_, {holder(), b.short_(0)}),
                b.begin({
                    b.call(op<BC>(mode_op::score_add), type_t::void_, {holder(), b.short_(1)}),
                    b.if_(
                        b.call(
                            BC::gequals,
                            type_t::bool_,
                            {b.call(op<BC>(mode_op::leader_score), type_t::short_, {}),
                             b.global("score_limit", type_t::short_)}),
                        b.call(op<BC>(mode_op::game_over), type_t::void_, {})),
                })),
        }));
    auto const index = [&b] { return b.call(op<BC>(mode_op::hill_index), type_t::short_, {}); };
    auto const count = [&b] { return b.call(op<BC>(mode_op::hill_count), type_t::short_, {}); };
    b.script(
        "move",
        script_type_t::continuous,
        b.if_(
            b.call(
                BC::and_,
                type_t::bool_,
                {b.global("moving_hill", type_t::bool_), b.call(BC::greater, type_t::bool_, {count(), b.short_(1)})}),
            b.begin({
                b.call(
                    BC::sleep,
                    type_t::void_,
                    {b.call(BC::mul_, type_t::real_, {b.global("hill_move_time", type_t::real_), b.real(30.f)})}),
                b.if_(
                    b.call(
                        BC::gequals,
                        type_t::bool_,
                        {b.call(BC::add_, type_t::short_, {index(), b.short_(1)}), count()}),
                    b.call(op<BC>(mode_op::hill_set), type_t::void_, {b.short_(0)}),
                    b.call(
                        op<BC>(mode_op::hill_set),
                        type_t::void_,
                        {b.call(BC::add_, type_t::short_, {index(), b.short_(1)})})),
            })));
    return std::move(b).build();
}

/* Race: the track is the race_track flags in order. Reaching the one a
 * player goes for moves them on; past the last they have run a lap and
 * score. First to the laps wins */
template<typename BC>
hsc_builder::program_t<BC> race()
{
    using hsc_builder::script_type_t;
    using hsc_builder::type_t;
    hsc_builder::builder<BC> b;
    auto const player = [&b] { return b.global("event_player", type_t::short_); };

    b.declare_global("score_limit", type_t::short_, b.short_(3)); /* laps */
    b.declare_global("respawn_delay", type_t::real_, b.real(3.f));
    b.declare_global("event_killer", type_t::short_, b.short_(-1));
    b.declare_global("event_victim", type_t::short_, b.short_(-1));
    b.declare_global("event_player", type_t::short_, b.short_(-1));

    b.script(
        "setup",
        script_type_t::startup,
        b.begin({
            b.call(op<BC>(mode_op::respawn_delay_set), type_t::void_, {b.global("respawn_delay", type_t::real_)}),
            b.call(op<BC>(mode_op::time_limit_set), type_t::void_, {b.real(0.f)}),
        }));
    b.script(
        "on_checkpoint",
        script_type_t::dormant,
        b.begin({
            b.call(op<BC>(mode_op::race_advance), type_t::void_, {player()}),
            b.if_(
                b.call(
                    BC::equal,
                    type_t::bool_,
                    {b.call(op<BC>(mode_op::race_next), type_t::short_, {player()}), b.short_(0)}),
                b.begin({
                    b.call(op<BC>(mode_op::score_add), type_t::void_, {player(), b.short_(1)}),
                    b.if_(
                        b.call(
                            BC::gequals,
                            type_t::bool_,
                            {b.call(op<BC>(mode_op::leader_score), type_t::short_, {}),
                             b.global("score_limit", type_t::short_)}),
                        b.call(op<BC>(mode_op::game_over), type_t::void_, {})),
                })),
        }));
    return std::move(b).build();
}

/* An announcer line, played here */
template<typename BC>
auto announce_line(hsc_builder::builder<BC>& b, announcer sound)
{
    return b.call(
        op<BC>(mode_op::announce), hsc_builder::type_t::void_, {b.short_(static_cast<libc_types::i16>(sound))});
}

/* What every announcer says: multi-kills and sprees to the player who made
 * them, and the game's end */
template<typename BC>
void announce_kills(hsc_builder::builder<BC>& b)
{
    using hsc_builder::script_type_t;
    using hsc_builder::type_t;
    using i16 = libc_types::i16;
    auto const player = [&b] { return b.global("event_player", type_t::short_); };
    auto const recent = [&](i16 n) {
        return b.call(
            BC::equal,
            type_t::bool_,
            {b.call(
                 op<BC>(mode_op::kills_within),
                 type_t::short_,
                 {player(), b.global("multi_kill_seconds", type_t::real_)}),
             b.short_(n)});
    };
    auto const streak = [&](std::string_view count) {
        return b.call(
            BC::equal,
            type_t::bool_,
            {b.call(op<BC>(mode_op::kill_streak), type_t::short_, {player()}), b.global(count, type_t::short_)});
    };
    b.declare_global("event_player", type_t::short_, b.short_(-1));
    b.declare_global("multi_kill_seconds", type_t::real_, b.real(multi_kill_seconds));
    b.declare_global("killing_spree_kills", type_t::short_, b.short_(5));
    b.declare_global("running_riot_kills", type_t::short_, b.short_(10));
    b.script(
        "on_local_kill",
        script_type_t::dormant,
        b.begin({
            b.if_(
                recent(2),
                announce_line(b, announcer::double_kill),
                b.if_(
                    recent(3),
                    announce_line(b, announcer::triple_kill),
                    b.if_(recent(4), announce_line(b, announcer::killtacular)))),
            b.if_(
                streak("killing_spree_kills"),
                announce_line(b, announcer::killing_spree),
                b.if_(streak("running_riot_kills"), announce_line(b, announcer::running_riot))),
        }));
    b.script("on_game_over", script_type_t::dormant, announce_line(b, announcer::game_over));
}

/* Slayer's announcer: the game type as it starts or is joined */
template<typename BC>
hsc_builder::program_t<BC> slayer_announcer()
{
    using hsc_builder::script_type_t;
    using hsc_builder::type_t;
    hsc_builder::builder<BC> b;
    announce_kills(b);
    b.script(
        "announce_game",
        script_type_t::startup,
        b.if_(
            b.call(op<BC>(mode_op::teams), type_t::bool_, {}),
            announce_line(b, announcer::team_slayer),
            announce_line(b, announcer::slayer)));
    return std::move(b).build();
}

/* Capture the flag's announcer: a flag taken, back home or captured, as
 * every peer sees the flags and team scores change */
template<typename BC>
hsc_builder::program_t<BC> ctf_announcer()
{
    using hsc_builder::script_type_t;
    using hsc_builder::type_t;
    using i16 = libc_types::i16;
    hsc_builder::builder<BC> b;
    announce_kills(b);
    auto const taken = [&b](i16 t) { return b.call(op<BC>(mode_op::flag_taken), type_t::bool_, {b.short_(t)}); };
    auto const score = [&b](i16 t) { return b.call(op<BC>(mode_op::team_score), type_t::short_, {b.short_(t)}); };
    auto const was_taken = [&b](i16 t) { return b.global(t == 0 ? "red_taken" : "blue_taken", type_t::bool_); };
    auto const had       = [&b](i16 t) { return b.global(t == 0 ? "red_score" : "blue_score", type_t::short_); };
    auto const remember  = [&] {
        return b.begin({
            b.set("red_taken", type_t::bool_, taken(0)),
            b.set("blue_taken", type_t::bool_, taken(1)),
            b.set("red_score", type_t::short_, score(0)),
            b.set("blue_score", type_t::short_, score(1)),
        });
    };
    b.declare_global("red_taken", type_t::bool_, b.boolean(false));
    b.declare_global("blue_taken", type_t::bool_, b.boolean(false));
    b.declare_global("red_score", type_t::short_, b.short_(0));
    b.declare_global("blue_score", type_t::short_, b.short_(0));
    b.script(
        "announce_game",
        script_type_t::startup,
        b.begin({remember(), announce_line(b, announcer::capture_the_flag)}));
    /* A flag taken is the other team having it; one back home without its
     * takers scoring was returned */
    auto const flag_lines = [&](i16 flag) {
        i16 const  takers = 1 - flag;
        bool const red    = flag == 0;
        return b.begin({
            b.if_(
                b.call(BC::and_, type_t::bool_, {taken(flag), b.call(BC::not_, type_t::bool_, {was_taken(flag)})}),
                announce_line(b, red ? announcer::blue_team_has_the_flag : announcer::red_team_has_the_flag)),
            b.if_(
                b.call(BC::greater, type_t::bool_, {score(takers), had(takers)}),
                announce_line(b, red ? announcer::blue_team_score : announcer::red_team_score),
                b.if_(
                    b.call(BC::and_, type_t::bool_, {b.call(BC::not_, type_t::bool_, {taken(flag)}), was_taken(flag)}),
                    announce_line(b, red ? announcer::red_team_flag_returned : announcer::blue_team_flag_returned))),
        });
    };
    b.script("watch_flags", script_type_t::continuous, b.begin({flag_lines(0), flag_lines(1), remember()}));
    /* While a flag is away its takers are reminded they have it */
    for(i16 flag = 0; flag < 2; flag++)
        b.script(
            flag == 0 ? "remind_red" : "remind_blue",
            script_type_t::continuous,
            b.if_(
                taken(flag),
                b.begin({
                    b.call(BC::sleep, type_t::void_, {b.short_(flag_reminder_ticks)}),
                    b.if_(
                        taken(flag),
                        announce_line(b, flag == 0 ? announcer::blue_team_has_the_flag : announcer::red_team_has_the_flag)),
                })));
    return std::move(b).build();
}

/* Oddball's announcer: the game type as it starts or is joined */
template<typename BC>
hsc_builder::program_t<BC> oddball_announcer()
{
    using hsc_builder::script_type_t;
    using hsc_builder::type_t;
    hsc_builder::builder<BC> b;
    announce_kills(b);
    b.script(
        "announce_game",
        script_type_t::startup,
        b.if_(
            b.call(op<BC>(mode_op::teams), type_t::bool_, {}),
            announce_line(b, announcer::team_oddball),
            announce_line(b, announcer::oddball)));
    return std::move(b).build();
}

/* King of the hill's announcer: the hill taken, fought over or moved, as
 * every peer sees it */
template<typename BC>
hsc_builder::program_t<BC> koth_announcer()
{
    using hsc_builder::script_type_t;
    using hsc_builder::type_t;
    hsc_builder::builder<BC> b;
    announce_kills(b);
    auto const holder = [&b] { return b.call(op<BC>(mode_op::hill_holder), type_t::short_, {}); };
    auto const index  = [&b] { return b.call(op<BC>(mode_op::hill_index), type_t::short_, {}); };
    b.declare_global("last_holder", type_t::short_, b.short_(-1));
    b.declare_global("last_hill", type_t::short_, b.short_(-1));
    b.script(
        "announce_game",
        script_type_t::startup,
        b.begin({
            b.set("last_holder", type_t::short_, holder()),
            b.set("last_hill", type_t::short_, index()),
            b.if_(
                b.call(op<BC>(mode_op::teams), type_t::bool_, {}),
                announce_line(b, announcer::team_king_of_the_hill),
                announce_line(b, announcer::king_of_the_hill)),
        }));
    auto const changed = [&](auto now, char const* was) {
        return b.call(BC::not_, type_t::bool_, {b.call(BC::equal, type_t::bool_, {now, b.global(was, type_t::short_)})});
    };
    b.script(
        "watch_hill",
        script_type_t::continuous,
        b.begin({
            b.if_(
                b.call(
                    BC::and_,
                    type_t::bool_,
                    {changed(index(), "last_hill"),
                     b.call(BC::gequals, type_t::bool_, {b.global("last_hill", type_t::short_), b.short_(0)})}),
                announce_line(b, announcer::hill_move)),
            b.if_(
                changed(holder(), "last_holder"),
                b.if_(
                    b.call(BC::equal, type_t::bool_, {holder(), b.short_(-2)}),
                    announce_line(b, announcer::hill_contested),
                    b.if_(
                        b.call(BC::gequals, type_t::bool_, {holder(), b.short_(0)}),
                        announce_line(b, announcer::hill_controlled)))),
            b.set("last_holder", type_t::short_, holder()),
            b.set("last_hill", type_t::short_, index()),
        }));
    return std::move(b).build();
}

/* Race's announcer: the game type as it starts or is joined */
template<typename BC>
hsc_builder::program_t<BC> race_announcer()
{
    using hsc_builder::script_type_t;
    using hsc_builder::type_t;
    hsc_builder::builder<BC> b;
    announce_kills(b);
    b.script(
        "announce_game",
        script_type_t::startup,
        b.if_(
            b.call(op<BC>(mode_op::teams), type_t::bool_, {}),
            announce_line(b, announcer::team_race),
            announce_line(b, announcer::race)));
    return std::move(b).build();
}

/* The announcer for a game type */
template<typename BC>
hsc_builder::program_t<BC> announcer_for(GameSetup::game_type_t type)
{
    switch(type)
    {
    case GameSetup::game_type_t::ctf:
        return ctf_announcer<BC>();
    case GameSetup::game_type_t::oddball:
        return oddball_announcer<BC>();
    case GameSetup::game_type_t::koth:
        return koth_announcer<BC>();
    case GameSetup::game_type_t::race:
        return race_announcer<BC>();
    default:
        return slayer_announcer<BC>();
    }
}

/* The rules for a game type; ones not written yet play as Slayer */
template<typename BC>
hsc_builder::program_t<BC> rules_for(GameSetup const& setup)
{
    switch(setup.game_type)
    {
    case GameSetup::game_type_t::ctf:
        return ctf<BC>(setup);
    case GameSetup::game_type_t::oddball:
        return oddball<BC>();
    case GameSetup::game_type_t::koth:
        return koth<BC>();
    case GameSetup::game_type_t::race:
        return race<BC>();
    default:
        return slayer<BC>(25, 0.f);
    }
}

} // namespace game_modes
