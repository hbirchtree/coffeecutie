#pragma once

#include "data.h"

struct SoundEvent
{
    enum type_t
    {
        loop_sound,
        play_sound,
        clear_all,
    } type;
    libc_types::u64 entity_id{0};
};

struct LoopSoundEvent
{
    static constexpr auto event_type = SoundEvent::loop_sound;

    blam::tagref_t const* sound{};
};

struct PlaySoundEvent
{
    static constexpr auto event_type = SoundEvent::play_sound;

    blam::tagref_t const* sound{};
    compo::time_point     time{};
};

void alloc_sound_system(compo::EntityContainer& e);
