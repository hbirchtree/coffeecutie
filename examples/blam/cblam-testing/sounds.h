#pragma once

#include "data.h"

struct SoundEvent
{
    enum type_t
    {
        loop_sound,
        play_sound,
        clear_all,
        stop_sound,
        background_sound_transition,
    } type;

    libc_types::u64 entity_id{0};
};

struct LoopSoundEvent
{
    static constexpr auto event_type = SoundEvent::loop_sound;

    enum class usage_t
    {
        general,
        background_track,
    };

    blam::tagref_t const* sound{};
    usage_t               usage{usage_t::general};
};

struct PlaySoundEvent
{
    static constexpr auto event_type = SoundEvent::play_sound;

    blam::tagref_t const* sound{};
    compo::time_point     time{};
};

struct BackgroundSoundTransitionEvent
{
    static constexpr auto event_type = SoundEvent::background_sound_transition;

    /* nullptr = fade out only, no new track */
    blam::tagref_t const* sound{nullptr};
};

struct SoundPreferences : compo::SubsystemBase
{
    using type = SoundPreferences;

    f32 master_volume{1.f};
};

void alloc_sound_system(compo::EntityContainer& e);
