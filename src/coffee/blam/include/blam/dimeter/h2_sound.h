#pragma once

#include "h2_structures.h"

#include <peripherals/identify/compiler/struct_packing.h>

namespace blam::dimeter::snd {

enum class sample_rate_t : u8
{
    rate_22khz = 0,
    rate_44khz = 1,
    rate_32khz = 2,
};

enum class encoding_t : u8
{
    mono   = 0,
    stereo = 1,
    codec  = 2,
};

enum class compression_t : u8
{
    pcm_be     = 0,
    xbox_adpcm = 1,
    ima_adpcm  = 2,
    pcm_le     = 3,
    wma        = 4,
};

inline u32 to_hertz(sample_rate_t rate)
{
    switch(rate)
    {
    case sample_rate_t::rate_22khz:
        return 22050;
    case sample_rate_t::rate_44khz:
        return 44100;
    case sample_rate_t::rate_32khz:
        return 32000;
    }
    return 0;
}

/*!
 * \brief snd! tag body. Shrunk from Halo 1's self-contained sound to 20
 * bytes: playback settings, pitch ranges and sample chunks all moved to the
 * per-map ugh! gestalt, referenced by index. Layout from Assembly's snd!.xml.
 */
struct alignas(4) sound
{
    u16           flags;
    u8            sound_class;
    sample_rate_t sample_rate;
    encoding_t    encoding;
    compression_t compression;

    i16 playback_index;
    i16 pitch_range_index;
    i8  unknown;
    i8  scale_index;
    i8  promotion_index;
    i8  custom_playback_index;
    i16 extra_info_index;
    i32 max_play_time;
};

static_assert(offsetof(sound, pitch_range_index) == 0x8);
static_assert(sizeof(sound) == 0x14);

/* Gestalt blocks the extraction path doesn't need yet; sized so tag_block
 * counts stay meaningful */
struct playback_parameters_t
{
    u32 opaque[0x38 / 4];
};

struct scale_t
{
    u32 opaque[0x14 / 4];
};

struct import_name_t
{
    string_id name;
};

struct alignas(2) pitch_range_parameter_t
{
    i16 natural_pitch;
    i16 bend_bounds[2];
    i16 max_gain_pitch_bounds[2];
};

static_assert(sizeof(pitch_range_parameter_t) == 0xA);

struct alignas(2) pitch_range_t
{
    i16 import_name_index;
    i16 parameter_index;
    i16 encoded_permutation_data_index;
    i16 encoded_runtime_flag_index;
    i16 first_permutation;
    i16 permutation_count;
};

static_assert(sizeof(pitch_range_t) == 0xC);

/* Packed: the wire layout has no padding anyway, but alignas(2) would be an
 * illegal alignment reduction (u32 member -> natural 4) that clang on
 * Android/wasm rejects; packing also keeps 2-aligned blocks safe to read */
PACKEDSTRUCT(permutation_t {
    i16 import_name_index;
    i16 encoded_skip_fraction;
    i8  gain;
    i8  info_index;
    i16 language_neutral_time;
    u32 sample_size_raw; /*!< Sum of this permutation's chunk sizes */
    i16 first_chunk;
    i16 chunk_count;

    inline u32 sample_size() const
    {
        return from_le(sample_size_raw);
    }
});

static_assert(sizeof(permutation_t) == 0x10);

/*!
 * \brief One contiguous piece of encoded sample data. Assembly labels the
 * fields at 0x4 as u16 flags + u16 size, but on real maps the size is a u32
 * with flags in the top bits (chunk sizes reach several MB for music, and
 * per-permutation sizes only add up with the full 30 bits).
 */
struct alignas(4) chunk_t
{
    raw_ref offset;
    u32     size_raw;
    i32     runtime_index;

    inline u32 size() const
    {
        return from_le(size_raw) & 0x3FFFFFFF;
    }
};

static_assert(sizeof(chunk_t) == 0xC);

struct custom_playback_t
{
    u32 opaque[0x34 / 4];
};

struct promotion_t
{
    u32 opaque[0x1C / 4];
};

struct extra_info_t
{
    u32 opaque[0x2C / 4];
};

/*!
 * \brief ugh! (sound gestalt) tag body: one per map, aggregates the sample
 * metadata for every sound in it. Chain for getting at sample data:
 * sound.pitch_range_index -> pitch_ranges[i] -> permutations
 * [first_permutation .. +permutation_count] -> chunks[first_chunk ..
 * +chunk_count] -> raw_ref into this or a shared map.
 */
struct alignas(4) gestalt
{
    tag_block<playback_parameters_t>   playbacks;
    tag_block<scale_t>                 scales;
    tag_block<import_name_t>           import_names;
    tag_block<pitch_range_parameter_t> pitch_range_parameters;
    tag_block<pitch_range_t>           pitch_ranges;
    tag_block<permutation_t>           permutations;
    tag_block<custom_playback_t>       custom_playbacks;
    tag_block<u8>                      runtime_permutation_flags;
    tag_block<chunk_t>                 chunks;
    tag_block<promotion_t>             promotions;
    tag_block<extra_info_t>            extra_infos;
};

static_assert(offsetof(gestalt, pitch_ranges) == 0x20);
static_assert(offsetof(gestalt, chunks) == 0x40);
static_assert(sizeof(gestalt) == 0x58);

} // namespace blam::dimeter::snd
