#pragma once

#include "blam_files.h"
#include "components.h"
#include "selected_version.h"
#include "ui_game_setup.h"

#include <algorithm>
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

/* The facing a teleport turns the camera to; its inverse, pitch left out */
inline f32 facing_of(PlayerCamera const& cam)
{
    Vecf3 const side = glm::conjugate(cam.camera.rotation) * Vecf3{1.f, 0.f, 0.f};
    return glm::pi<f32>() - std::atan2(side.z, side.x);
}

/* Of the start points for this game type and team, one nobody stands on
 * among the few furthest from `enemies`; none free and it waits */
inline spot_t pick(
    BlamFiles<halo_version> const& files,
    std::mt19937&                  random,
    std::vector<Vecf3> const&      enemies,
    blam::scn::gamemode_t          mode     = blam::scn::gamemode_t::none,
    i16                            team     = -1,
    std::vector<Vecf3> const*      standing = nullptr)
{
    auto const* scenario = files.container.scenario().value_or(nullptr);
    if(!scenario)
        return std::nullopt;
    auto spots = scenario->player_start.locations.data(files.container.magic);
    if(!spots.has_value() || spots.value().empty())
        return std::nullopt;
    using gm = blam::scn::gamemode_t;
    std::vector<std::pair<f32, size_t>> ranked;
    bool                                any_fit = false;
    for(size_t i = 0; i < spots.value().size(); i++)
    {
        auto const& spot = spots.value()[i];
        bool const  fits = mode == gm::none ||
                          std::any_of(std::begin(spot.types), std::end(spot.types), [mode](gm t) {
                              return t == mode || t == gm::all_games;
                          });
        bool const ours = team < 0 || spot.team_index == static_cast<u16>(team);
        bool const taken =
            standing && std::any_of(standing->begin(), standing->end(), [&spot](Vecf3 const& at) {
                Vecf3 const d = at - spot.pos;
                return glm::length(Vecf2(d)) < clearance && std::abs(d.z) < height;
            });
        any_fit = any_fit || (fits && ours);
        if(!fits || !ours || taken)
            continue;
        f32 nearest = std::numeric_limits<f32>::max();
        for(Vecf3 const& enemy : enemies)
            nearest = std::min(nearest, glm::distance(enemy, spot.pos));
        ranked.push_back({nearest, i});
    }
    if(ranked.empty() && any_fit)
        return std::nullopt;
    if(ranked.empty())
        for(size_t i = 0; i < spots.value().size(); i++)
            ranked.push_back({0.f, i});
    std::sort(ranked.begin(), ranked.end(), std::greater<>());
    size_t const choice = std::uniform_int_distribution<size_t>(
        0, std::min<size_t>(ranked.size(), choices) - 1)(random);
    auto const& spot = spots.value()[ranked[choice].second];
    return std::pair{spot.pos, spot.rot};
}

} // namespace spawn_points
