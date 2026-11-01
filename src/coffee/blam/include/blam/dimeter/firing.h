#pragma once

#include "map.h"
#include "objects.h"

#include <algorithm>
#include <vector>

/* Halo 2's side of the firing overloads (see volta/blam_weapon_firing.h):
 * Halo 1's trigger is split into a trigger and the barrel it fires, and the
 * barrel holds the rate, rounds and magazine */
namespace blam::dimeter::obj {

/*! A trigger and its primary barrel */
struct firing_t
{
    trigger_t const* trigger{nullptr};
    barrel_t const*  barrel{nullptr};
};

/*! The weapon's triggers that fire a barrel, primary first */
template<typename V>
inline std::vector<firing_t> firing_modes(weapon_t const& w, map_container<V> const& map)
{
    std::vector<firing_t> out;
    auto const            barrels = map.span(w.barrels);
    for(auto const& trigger : map.span(w.triggers))
        if(auto const* barrel = trigger.primary_barrel.resolve(barrels))
            out.push_back({&trigger, barrel});
    return out;
}

inline f32 rate_of_fire(firing_t const& f)
{
    f32 const most = from_le(f.barrel->rounds_per_second.y);
    return most > 0.f ? most : from_le(f.barrel->rounds_per_second.x);
}

/*! Only a spewing trigger fires again while held */
inline bool repeats(firing_t const& f)
{
    return rate_of_fire(f) > 0.f &&
           static_cast<trigger_behavior_t>(from_le(static_cast<u16>(f.trigger->behavior))) ==
               trigger_behavior_t::spew;
}

/*! Shots one pull fires: a burst, as the battle rifle's */
inline i16 shots_per_pull(firing_t const& f)
{
    return std::max<i16>(from_le(f.barrel->shots_per_fire[1]), 1);
}

inline i16 rounds_per_shot(firing_t const& f)
{
    return f.barrel->rounds_per_shot();
}

inline f32 charging_time(firing_t const& f)
{
    bool const charges =
        static_cast<trigger_behavior_t>(from_le(static_cast<u16>(f.trigger->behavior))) ==
        trigger_behavior_t::charge;
    return charges ? from_le(f.trigger->charging_time) : 0.f;
}

inline f32 illumination_recovery_time(firing_t const& f)
{
    return from_le(f.barrel->illumination_recovery_time);
}

inline f32 heat_per_round(firing_t const& f)
{
    return from_le(f.barrel->heat_generated_per_round);
}

inline f32 age_per_round(firing_t const& f)
{
    return from_le(f.barrel->age_generated_per_round);
}

/*! The magazine a barrel loads from; none for one that loads no rounds */
template<typename V>
inline magazine_t const* magazine_of(
    weapon_t const& w, firing_t const& f, map_container<V> const& map)
{
    auto const* magazine = f.barrel->magazine.resolve(map.span(w.magazines));
    return magazine && magazine->rounds_loaded_maximum() > 0 ? magazine : nullptr;
}

inline i16 rounds_loaded_maximum(magazine_t const& magazine)
{
    return magazine.rounds_loaded_maximum();
}

inline f32 reload_time(magazine_t const& magazine)
{
    return from_le(magazine.reload_time);
}

inline f32 heat_loss_rate(weapon_t const& w)
{
    return from_le(w.heat_loss_per_second);
}

inline f32 heat_recovery_threshold(weapon_t const& w)
{
    return from_le(w.heat_recovery_threshold);
}

inline f32 overheated_threshold(weapon_t const& w)
{
    return from_le(w.overheated_threshold);
}

inline bool stops_at_maximum_age(weapon_t const&)
{
    return false;
}

} // namespace blam::dimeter::obj
