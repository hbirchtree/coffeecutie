#pragma once

#include "h2_scene.h"

#include <string>
#include <vector>

/* Halo 2 audio: the loader fills H2SoundBank from the map, DimeterSoundSystem
 * plays it. Nothing here points back at the loader or the shell */
namespace h2 {

/* One permutation's samples: Xbox ADPCM blocks as OpenAL's IMA4 takes
 * them, or a whole WMA file for the "codec" encoding */
struct H2Sound
{
    std::string       name;
    std::vector<char> blocks;
    bool              wma{false};
    u32               rate{44100};
    u16               channels{1};
};

/* A looping sound's first track: the intro once, then the loop */
struct H2LoopingSound
{
    std::string name;
    H2Sound     in;
    H2Sound     loop;
    f32         gain{1.f};
};

struct H2SoundBank : compo::SubsystemBase
{
    using type = H2SoundBank;

    bool loaded{false};
    u32  generation{0};

    /* The map's background music, when it has one (mainmenu's wigl) */
    H2LoopingSound music;
};

/* `api` is the audio system BlamGraphics already brought up, null without
 * sound */
void alloc_h2_sound(compo::EntityContainer& e, void* api);

} // namespace h2
