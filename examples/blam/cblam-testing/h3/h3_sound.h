#pragma once

#include "h3_scene.h"

#include <string>
#include <vector>

/* Halo 3 audio: the loader fills H3SoundBank from the map, ElegySoundSystem
 * plays it */
namespace h3 {

/* One permutation's XMA2 data, decoded at play time */
struct H3Sound
{
    std::string       name;
    std::vector<char> data;
    u32               rate{44100};
    u32               channels{2};
    u32               samples{0}; /*!< Per channel */
};

/* A looping sound's first track: the intro once, then its loop
 * permutations in turn */
struct H3LoopingSound
{
    std::string          name;
    H3Sound              in;
    std::vector<H3Sound> loops;
    f32                  gain{1.f};
};

struct H3SoundBank : compo::SubsystemBase
{
    using type = H3SoundBank;

    bool loaded{false};
    u32  generation{0};

    /* The map's background music, when it has one (mainmenu's
     * main_menu_music) */
    H3LoopingSound music;
};

/* `api` is the audio system BlamGraphics already brought up, null without
 * sound */
void alloc_h3_sound(compo::EntityContainer& e, void* api);

} // namespace h3
