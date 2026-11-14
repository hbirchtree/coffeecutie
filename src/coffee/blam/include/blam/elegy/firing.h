#pragma once

#include "map.h"
#include "objects.h"

#include <algorithm>
#include <vector>

/* Halo 3's side of the firing overloads (see volta/blam_weapon_firing.h):
 * as Halo 2, a trigger fires a barrel, and the barrel holds the rate, rounds
 * and magazine */
namespace blam::elegy {

/*! A trigger and its primary barrel */
struct firing_t
{
    trigger_t const* trigger{nullptr};
    barrel_t const*  barrel{nullptr};
};

/*! The element `index` names, null when unset or out of range */
template<typename T>
inline T const* at(Span<T const> items, i16 index)
{
    return index >= 0 && static_cast<size_t>(index) < items.size() ? &items[static_cast<size_t>(index)]
                                                                     : nullptr;
}

/*! The weapon's triggers that fire a barrel, primary first */
inline std::vector<firing_t> firing_modes(weapon_t const& w, map_container const& map)
{
    std::vector<firing_t> out;
    auto const            barrels = map.span(w.barrels);
    for(auto const& trigger : map.span(w.triggers))
        if(auto const* barrel = at(barrels, trigger.primary_barrel.value()))
            out.push_back({&trigger, barrel});
    return out;
}

inline f32 rate_of_fire(firing_t const& f)
{
    Vecf2 const rate = f.barrel->rounds_per_second.value();
    return rate.y > 0.f ? rate.y : rate.x;
}

/*! Only a spewing trigger fires again while held */
inline bool repeats(firing_t const& f)
{
    return rate_of_fire(f) > 0.f && f.trigger->behavior.value() == trigger_behavior_t::spew;
}

/*! Shots one pull fires: a burst, as the battle rifle's */
inline i16 shots_per_pull(firing_t const& f)
{
    return std::max<i16>(f.barrel->shots_per_fire[1].value(), 1);
}

inline i16 rounds_per_shot(firing_t const& f)
{
    return f.barrel->rounds_per_shot.value();
}

inline f32 charging_time(firing_t const& f)
{
    return f.trigger->behavior.value() == trigger_behavior_t::charge ? f.trigger->charging_time.value()
                                                                     : 0.f;
}

inline f32 illumination_recovery_time(firing_t const& f)
{
    return f.barrel->illumination_recovery_time.value();
}

inline f32 heat_per_round(firing_t const& f)
{
    return f.barrel->heat_generated_per_round.value();
}

inline f32 age_per_round(firing_t const& f)
{
    return f.barrel->age_generated_per_round.value();
}

/*! The magazine a barrel loads from; none for one that loads no rounds */
inline magazine_t const* magazine_of(weapon_t const& w, firing_t const& f, map_container const& map)
{
    auto const* magazine = at(map.span(w.magazines), f.barrel->magazine.value());
    return magazine && magazine->rounds_loaded_maximum.value() > 0 ? magazine : nullptr;
}

inline i16 rounds_loaded_maximum(magazine_t const& magazine)
{
    return magazine.rounds_loaded_maximum.value();
}

inline f32 reload_time(magazine_t const& magazine)
{
    return magazine.reload_time.value();
}

inline f32 heat_loss_rate(weapon_t const& w)
{
    return w.heat_loss_per_second.value();
}

inline f32 heat_recovery_threshold(weapon_t const& w)
{
    return w.heat_recovery_threshold.value();
}

inline f32 overheated_threshold(weapon_t const& w)
{
    return w.overheated_threshold.value();
}

inline bool stops_at_maximum_age(weapon_t const&)
{
    return false;
}

} // namespace blam::elegy
