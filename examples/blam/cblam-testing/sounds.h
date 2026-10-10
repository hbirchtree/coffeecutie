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
        update_sound,
    } type;

    libc_types::u64 entity_id{0};

    /* entity_id range for scenario sound scenery ('SSCE' << 32) */
    static constexpr libc_types::u64 sound_scenery_base =
        0x5353434500000000ULL;
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

/*! Moves a looping sound for `entity_id`, scales its gain and pitch */
struct UpdateSoundEvent
{
    static constexpr auto event_type = SoundEvent::update_sound;

    Vecf3           position{};
    libc_types::f32 gain{1.f};
    libc_types::f32 pitch{1.f};
};

struct PlaySoundEvent
{
    static constexpr auto event_type = SoundEvent::play_sound;

    blam::tagref_t const* sound{};
    blam::tag_t const*    sound_tag{};
    compo::time_point     time{};

    /* relative = position is an offset from the listener, {0,0,0} = at the ear
     * Positioning only affects mono sounds */
    Vecf3                 position{};
    bool                  relative{false};
    bool                  looping{false};
};

struct BackgroundSoundTransitionEvent
{
    static constexpr auto event_type = SoundEvent::background_sound_transition;

    /* nullptr = fade out only, no new track */
    blam::tagref_t const* sound{nullptr};
    blam::tag_t const*    sound_tag{nullptr}; // special-case for title track for now
};

struct SoundPreferences : compo::SubsystemBase
{
    using type = SoundPreferences;

    f32 master_volume{1.f};
};

void alloc_sound_system(compo::EntityContainer& e, bool enabled);
