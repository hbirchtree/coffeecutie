#pragma once

#include "loading/blam_files.h"
#include "components.h"
#include "selected_version.h"
#include "ui/ui_game_setup.h"

#include <algorithm>
#include <functional>
#include <optional>
#include <random>
#include <vector>

/* Where players are put back into the game: the scenario's start points */
namespace spawn_points {

using libc_types::f32;
using libc_types::i16;
using libc_types::u16;

/*! Feet and facing */
using spot_t = std::optional<std::pair<Vecf3, f32>>;

/* A start point is taken while anyone stands this close to it */
constexpr f32    clearance = 1.f; /* wu */
constexpr f32    height    = 1.f; /* wu */
constexpr size_t choices   = 3;
constexpr f32    enemy_reach = 10.f; /* wu; an enemy closer makes a spot unsafe */
constexpr f32    death_reach = 6.f;  /* wu; so does someone just killed there */
constexpr f32    sight_reach = 40.f; /* wu an enemy's view counts out to */
constexpr f32    sight_cos   = .6f;  /* about 53 degrees off where an enemy looks */

inline blam::scn::gamemode_t game_mode_of(GameSetup::game_type_t type)
{
    using t  = GameSetup::game_type_t;
    using gm = blam::scn::gamemode_t;
    switch(type)
    {
    case t::ctf:
        return gm::ctf;
    case t::koth:
        return gm::king_of_the_hill;
    case t::oddball:
        return gm::oddball;
    case t::race:
        return gm::race;
    default:
        return gm::slayer;
    }
}

/* Whether something placed for `placed` is in a game of `mode` */
inline bool fits(blam::scn::gamemode_t placed, blam::scn::gamemode_t mode)
{
    using gm = blam::scn::gamemode_t;
    return placed == mode || placed == gm::all_games || (placed == gm::all_except_ctf && mode != gm::ctf) ||
           (placed == gm::all_except_race_ctf && mode != gm::ctf && mode != gm::race);
}

/* A placed vehicle's default sets by game type; race has none of its own and
 * takes slayer's. Xbox maps set no flags: there every one is placed */
inline bool vehicle_in(blam::scn::vehicle_spawn::multiplayer_spawn_flags_t flags, GameSetup::game_type_t type)
{
    using f = blam::scn::vehicle_spawn::multiplayer_spawn_flags_t;
    using t = GameSetup::game_type_t;
    if(flags == f::none_spawn)
        return true;
    f const wanted = type == t::ctf       ? f::ctf_default
                     : type == t::koth    ? f::king_default
                     : type == t::oddball ? f::oddball_default
                                          : f::slayer_default;
    return (flags & wanted) != f::none_spawn;
}

/* The facing a teleport turns the camera to; its inverse, pitch left out */
inline f32 facing_of(PlayerCamera const& cam)
{
    Vecf3 const side = glm::conjugate(cam.camera.rotation) * Vecf3{1.f, 0.f, 0.f};
    return glm::pi<f32>() - std::atan2(side.z, side.x);
}

/*! What the picker weighs: who is about, and where it last put this player */
struct around_t
{
    std::vector<Vecf3> enemies;
    std::vector<Vecf3> standing; /*!< Anyone alive, who blocks a spot */
    std::vector<Vecf3> deaths;   /*!< Where bodies just fell */
    std::optional<size_t> last;  /*!< Not the same spot twice running */
    /*! Enemies' eyes and where they look */
    std::vector<std::pair<Vecf3, Vecf3>> views;
    /*! Whether nothing stands between two points */
    std::function<bool(Vecf3 const&, Vecf3 const&)> clear;
};

/* In view of an enemy: within its reach and view cone, unobstructed */
inline bool seen(around_t const& around, Vecf3 const& at)
{
    Vecf3 const chest = at + Vecf3{0.f, 0.f, height * .6f};
    return std::any_of(around.views.begin(), around.views.end(), [&](auto const& view) {
        Vecf3 const to = chest - view.first;
        f32 const   d  = glm::length(to);
        if(d > sight_reach || d < 1e-3f || glm::dot(to / d, view.second) < sight_cos)
            return false;
        return !around.clear || around.clear(view.first, chest);
    });
}

/* Of the start points for this game type and team that nobody stands on,
 * any safe one at random: no enemy near or looking at it, nobody just
 * killed close by, not where it last put this player. None safe: one of the
 * few out of sight and furthest from the enemy. None free and it waits */
inline std::optional<std::pair<spot_t, size_t>> pick(
    BlamFiles<halo_version> const& files,
    std::mt19937&                  random,
    around_t const&                around,
    blam::scn::gamemode_t          mode = blam::scn::gamemode_t::none,
    i16                            team = -1)
{
    auto const* scenario = files.container.scenario().value_or(nullptr);
    if(!scenario)
        return std::nullopt;
    auto spots = scenario->player_start.locations.data(files.container.magic);
    if(!spots.has_value() || spots.value().empty())
        return std::nullopt;
    using gm = blam::scn::gamemode_t;
    auto const within = [](std::vector<Vecf3> const& them, Vecf3 const& at, f32 reach) {
        return std::any_of(them.begin(), them.end(), [&](Vecf3 const& p) { return glm::distance(p, at) < reach; });
    };
    std::vector<std::pair<f32, size_t>> ranked;
    std::vector<size_t>                 safe;
    bool                                any_fit = false;
    for(size_t i = 0; i < spots.value().size(); i++)
    {
        auto const& spot = spots.value()[i];
        bool const  fits = mode == gm::none ||
                          std::any_of(std::begin(spot.types), std::end(spot.types), [mode](gm t) {
                              return spawn_points::fits(t, mode);
                          });
        bool const ours  = team < 0 || spot.team_index == static_cast<u16>(team);
        bool const taken = std::any_of(around.standing.begin(), around.standing.end(), [&spot](Vecf3 const& at) {
            Vecf3 const d = at - spot.pos;
            return glm::length(Vecf2(d)) < clearance && std::abs(d.z) < height;
        });
        any_fit = any_fit || (fits && ours);
        if(!fits || !ours || taken)
            continue;
        f32 nearest = std::numeric_limits<f32>::max();
        for(Vecf3 const& enemy : around.enemies)
            nearest = std::min(nearest, glm::distance(enemy, spot.pos));
        bool const watched = seen(around, spot.pos);
        /* Out of sight ranks ahead of any distance */
        ranked.push_back({nearest + (watched ? 0.f : 1e6f), i});
        if(nearest >= enemy_reach && !watched && !within(around.deaths, spot.pos, death_reach))
            safe.push_back(i);
    }
    if(ranked.empty() && any_fit)
        return std::nullopt;
    if(ranked.empty())
        for(size_t i = 0; i < spots.value().size(); i++)
            ranked.push_back({0.f, i});
    if(safe.size() > 1 && around.last)
        std::erase(safe, *around.last);
    size_t index = 0;
    if(!safe.empty())
        index = safe[std::uniform_int_distribution<size_t>(0, safe.size() - 1)(random)];
    else
    {
        std::sort(ranked.begin(), ranked.end(), std::greater<>());
        index = ranked[std::uniform_int_distribution<size_t>(0, std::min<size_t>(ranked.size(), choices) - 1)(random)]
                    .second;
    }
    auto const& spot = spots.value()[index];
    return std::pair{spot_t{std::pair{spot.pos, spot.rot}}, index};
}

} // namespace spawn_points
