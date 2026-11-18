#pragma once

#include "blam_stl.h"
#include "blam_unit.h"

/* Halo 1's side of the firing overloads: a firing mode is a trigger, which
 * holds its rate, rounds, magazine and heat. Other games overload the same
 * names for their own weapon tags */
namespace blam::scn {

/*! The weapon's triggers, primary first */
template<typename V>
inline semantic::Span<weapon::trigger_t const> firing_modes(
    weapon const& w, map_container<V> const& map)
{
    auto triggers = w.triggers.data(map.magic);
    return triggers.has_value() ? triggers.value() : semantic::Span<weapon::trigger_t const>{};
}

/*! Shots a second; zero fires once a pull */
inline f32 rate_of_fire(weapon::trigger_t const& trigger)
{
    return trigger.maximum_rate_of_fire.y > 0.f ? trigger.maximum_rate_of_fire.y
                                                : trigger.maximum_rate_of_fire.x;
}

/*! Fires again while held */
inline bool repeats(weapon::trigger_t const& trigger)
{
    using flags_t = weapon::trigger_t::flags_t;
    return rate_of_fire(trigger) > 0.f &&
           (trigger.flags & flags_t::does_not_repeat_automatically) == flags_t::none;
}

/*! Shots one pull fires; Halo 1 has no bursts */
inline i16 shots_per_pull(weapon::trigger_t const&)
{
    return 1;
}

inline i16 rounds_per_shot(weapon::trigger_t const& trigger)
{
    return trigger.rounds_per_shot;
}

/*! Seconds held to a full charge, zero when it does not charge */
inline f32 charging_time(weapon::trigger_t const& trigger)
{
    return trigger.charging_time;
}

inline f32 illumination_recovery_time(weapon::trigger_t const& trigger)
{
    return trigger.illumination_recovery_time;
}

inline f32 heat_per_round(weapon::trigger_t const& trigger)
{
    return trigger.heat_generated_per_round;
}

inline f32 age_per_round(weapon::trigger_t const& trigger)
{
    return trigger.age_generated_per_round;
}

/*! The magazine a trigger loads from; none for one that loads no rounds,
 * such as an energy weapon's */
template<typename V>
inline weapon::magazine_t const* magazine_of(
    weapon const& w, weapon::trigger_t const& trigger, map_container<V> const& map)
{
    auto magazines = w.magazines.data(map.magic);
    if(trigger.magazine < 0 || !magazines.has_value() ||
       static_cast<size_t>(trigger.magazine) >= magazines.value().size())
        return nullptr;
    auto const& magazine = magazines.value()[trigger.magazine];
    return magazine.rounds_loaded_maximum > 0 ? &magazine : nullptr;
}

inline i16 rounds_loaded_maximum(weapon::magazine_t const& magazine)
{
    return magazine.rounds_loaded_maximum;
}

inline f32 reload_time(weapon::magazine_t const& magazine)
{
    return magazine.reload_time;
}

/*! Heat lost a second */
inline f32 heat_loss_rate(weapon const& w)
{
    return w.heat_loss_rate;
}

/*! Heat a vented weapon fires again below */
inline f32 heat_recovery_threshold(weapon const& w)
{
    return w.heat_recovery_threshold;
}

/*! Heat it vents at; zero when it never does */
inline f32 overheated_threshold(weapon const& w)
{
    return w.overheated_threshold;
}

/*! A flat battery stops it */
inline bool stops_at_maximum_age(weapon const& w)
{
    using flags_t = weapon::weapon_flags_t;
    return (w.weapon_flags & flags_t::cannot_fire_at_maximum_age) != flags_t::none;
}

} // namespace blam::scn
