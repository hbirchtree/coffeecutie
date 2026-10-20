#pragma once

#include "resources.h"

#include <vector>

namespace blam::elegy {

enum class sample_rate_t : u8
{
    hz_22050 = 0,
    hz_44100 = 1,
    hz_32000 = 2,
};

enum class sound_encoding_t : u8
{
    mono     = 0,
    stereo   = 1,
    quad     = 2,
    surround = 3, /*!< 5.1 */
    codec    = 4,
};

enum class sound_compression_t : u8
{
    none_be    = 0,
    xbox_adpcm = 1,
    ima_adpcm  = 2,
    none_le    = 3,
    wma        = 4,
    none       = 5,
    xma        = 6,
    xma2       = 7, /*!< Everything in the retail 360 maps */
};

struct sound_codec_t
{
    sample_rate_t       sample_rate;
    sound_encoding_t    encoding;
    sound_compression_t compression;

    inline u32 rate() const
    {
        switch(sample_rate)
        {
        case sample_rate_t::hz_22050:
            return 22050;
        case sample_rate_t::hz_44100:
            return 44100;
        case sample_rate_t::hz_32000:
            return 32000;
        }
        return 0;
    }

    inline u32 channels() const
    {
        switch(encoding)
        {
        case sound_encoding_t::mono:
            return 1;
        case sound_encoding_t::stereo:
            return 2;
        case sound_encoding_t::quad:
            return 4;
        case sound_encoding_t::surround:
            return 6;
        default:
            return 0;
        }
    }
};

static_assert(sizeof(sound_codec_t) == 0x3);

struct sound_playback_t
{
    be<u32>   internal_flags;
    be<f32>   dont_play_distance;
    be<f32>   attack_distance;
    be<f32>   minimum_distance;
    be<f32>   maximum_distance;
    be<f32>   skip_fraction;
    be<f32>   max_bend_per_second;
    be<f32>   gain_base;
    be<f32>   gain_variance;
    be<i16>   random_pitch_min;
    be<i16>   random_pitch_max;
    be<f32>   inner_cone_angle;
    be<f32>   outer_cone_angle;
    be<f32>   outer_cone_gain;
    be<u32>   flags;
    be<f32>   azimuth;
    be<f32>   position_gain;
    be<f32>   first_person_gain;
};

static_assert(sizeof(sound_playback_t) == 0x44);

struct sound_scale_t
{
    be<f32> gain_min;
    be<f32> gain_max;
    be<i16> pitch_min;
    be<i16> pitch_max;
    be<f32> skip_fraction_min;
    be<f32> skip_fraction_max;
};

static_assert(sizeof(sound_scale_t) == 0x14);

struct sound_pitch_range_parameter_t
{
    be<i16> natural_pitch;
    be<i16> bend_min;
    be<i16> bend_max;
    be<i16> max_gain_pitch_min;
    be<i16> max_gain_pitch_max;
    be<i16> playback_pitch_min;
    be<i16> playback_pitch_max;
};

static_assert(sizeof(sound_pitch_range_parameter_t) == 0xE);

struct sound_permutation_t;

/*!
 * \brief A pitch range: its permutations are a run of the gestalt's
 * permutation block, packed as required:6 count:6 first:20.
 */
struct sound_pitch_range_t
{
    block_index<string_id>                     name;
    block_index<sound_pitch_range_parameter_t> parameter;
    be<u16>                                    encoded_permutation_data;
    be<i16>                                    runtime_permutation_flag;
    be<u32>                                    encoded_permutations;

    inline u32 first_permutation() const
    {
        return encoded_permutations.value() & 0xFFFFF;
    }

    inline u32 permutation_count() const
    {
        return (encoded_permutations.value() >> 20) & 0x3F;
    }

    inline u32 required_permutations() const
    {
        return encoded_permutations.value() >> 26;
    }
};

static_assert(sizeof(sound_pitch_range_t) == 0xC);

struct sound_chunk_t;

/*! One variation of a sound: a run of chunks in the sound's resource */
struct sound_permutation_t
{
    block_index<string_id> name;
    be<i16>                encoded_skip_fraction;
    be<u32>                sample_count; /*!< Per channel */
    be<i32>                first_chunk;
    be<i16>                chunk_count;
    u8                     encoded_gain;
    u8                     extra_info;
};

static_assert(sizeof(sound_permutation_t) == 0x10);

/*!
 * \brief Where a permutation's bytes are, as an offset into the sound
 * resource (primary page bytes, then secondary). The top byte of the size
 * word carries flags.
 */
struct sound_chunk_t
{
    be<u32> offset;
    be<u32> encoded_size;
    be<i32> cache_index;
    be<i32> xma_buffer_start;
    be<i32> xma_buffer_end;

    inline u32 size() const
    {
        return encoded_size.value() & 0xFFFFFF;
    }

    inline u8 flags() const
    {
        return encoded_size.value() >> 24;
    }
};

static_assert(sizeof(sound_chunk_t) == 0x14);

struct sound_language_duration_t
{
    be<i32>                language;
    tag_block<be<i16>>     permutation_durations;
    tag_block<std::array<be<i16>, 2>> pitch_ranges;
};

static_assert(sizeof(sound_language_duration_t) == 0x1C);

struct sound_promotion_rule_t
{
    be<i16> pitch_range;
    be<i16> maximum_playing_count;
    be<f32> suppression_time;
    be<i32> rollover_time;
    be<i32> impulse_time;
};

static_assert(sizeof(sound_promotion_rule_t) == 0x10);

struct sound_promotion_t
{
    tag_block<sound_promotion_rule_t> rules;
    tag_block<be<i32>>                runtime_timers;
    be<i32>                           active;
    be<i32>                           last_promotion_time;
    be<i32>                           suppression_timeout;
};

static_assert(sizeof(sound_promotion_t) == 0x24);

struct sound_dialogue_info_t
{
    be<i32> mouth_data_offset;
    be<i32> mouth_data_length;
    be<i32> lipsync_data_offset;
    be<i32> lipsync_data_length;
};

static_assert(sizeof(sound_dialogue_info_t) == 0x10);

struct sound_encoded_permutation_section_t
{
    data_reference                   data;
    tag_block<sound_dialogue_info_t> dialogue;
    tag_block<std::array<u8, 0xC>>   unknown_0;
};

static_assert(sizeof(sound_encoded_permutation_section_t) == 0x2C);

struct sound_extra_info_t
{
    tag_block<sound_encoded_permutation_section_t> encoded_sections;
};

static_assert(sizeof(sound_extra_info_t) == 0xC);

/*!
 * \brief sound_cache_file_gestalt ("ugh!"): the per-map tables every sound
 * indexes into, by first index and count.
 */
struct sound_gestalt_t
{
    static constexpr tag_class_t group = tag_class_t::ugh;

    tag_block<sound_codec_t>                 codecs;
    tag_block<sound_playback_t>              playbacks;
    tag_block<sound_scale_t>                 scales;
    tag_block<string_id>                     names;
    tag_block<sound_pitch_range_parameter_t> pitch_range_parameters;
    tag_block<sound_pitch_range_t>           pitch_ranges;
    tag_block<sound_permutation_t>           permutations;
    tag_block<std::array<u8, 0x48>>          custom_playbacks;
    tag_block<sound_language_duration_t>     language_durations;
    tag_block<u8>                            runtime_permutation_flags;
    std::array<u32, 7>                       unknown_0;
    tag_block<sound_chunk_t>                 chunks;
    tag_block<sound_promotion_t>             promotions;
    tag_block<sound_extra_info_t>            extra_infos;
};

static_assert(offsetof(sound_gestalt_t, pitch_ranges) == 0x3C);
static_assert(offsetof(sound_gestalt_t, runtime_permutation_flags) == 0x6C);
static_assert(offsetof(sound_gestalt_t, chunks) == 0x94);
static_assert(offsetof(sound_gestalt_t, extra_infos) == 0xAC);

/*! sound ("snd!"): indices into the gestalt plus its data resource */
struct sound_t
{
    static constexpr tag_class_t group = tag_class_t::snd;

    be<u16>                                  flags;
    u8                                       sound_class;
    u8                                       pitch_range_count;
    block_index<sound_codec_t>               codec;
    block_index<sound_pitch_range_t>         first_pitch_range;
    be<i16>                                  first_language_pitch_range;
    be<i16>                                  sub_priority;
    block_index<sound_playback_t>            playback;
    block_index<sound_scale_t>               scale;
    i8                                       promotion;
    i8                                       custom_playback;
    block_index<sound_extra_info_t>          extra_info;
    be<i32>                                  max_playtime;
    datum_index                              resource;

    inline Span<sound_pitch_range_t const> pitch_ranges(
        sound_gestalt_t const& gestalt, tag_magic const& magic) const
    {
        auto all = gestalt.pitch_ranges.data(magic).value_or(
            Span<sound_pitch_range_t const>());
        i16 first = first_pitch_range.value();
        if(first < 0 || static_cast<size_t>(first) + pitch_range_count > all.size())
            return {};
        return all.subspan(static_cast<size_t>(first), pitch_range_count);
    }
};

static_assert(offsetof(sound_t, codec) == 0x4);
static_assert(offsetof(sound_t, extra_info) == 0x12);
static_assert(offsetof(sound_t, resource) == 0x18);

/*! A looping sound's track: intro, loop and outro sounds, played in turn */
struct looping_sound_track_t
{
    string_id     name;
    be<u32>       flags;
    be<f32>       gain; /*!< dB */
    be<f32>       fade_in;
    be<f32>       fade_out;
    tag_reference in;
    tag_reference loop;
    tag_reference out;
    tag_reference alternate_loop;
    tag_reference alternate_out;
    be<i16>       output_effect;
    u16           unknown_0;
    tag_reference alternate_transition_in;
    tag_reference alternate_transition_out;
    be<f32>       alternate_crossfade;
    be<f32>       alternate_fade_out;
};

static_assert(offsetof(looping_sound_track_t, loop) == 0x24);
static_assert(offsetof(looping_sound_track_t, alternate_transition_in) == 0x68);
static_assert(sizeof(looping_sound_track_t) == 0x90);

/*! sound_looping ("lsnd"): tracks in parallel plus random details */
struct looping_sound_t
{
    static constexpr tag_class_t group = tag_class_t::lsnd;

    be<u32>                          flags;
    bounds_f                         music_time;
    bounds_f                         distance;
    tag_reference                    continuous_damage_effect;
    be<i16>                          sound_class;
    u16                              unknown_0;
    tag_block<looping_sound_track_t> tracks;
    tag_block<std::array<u8, 0x3C>>  details;
};

static_assert(offsetof(looping_sound_t, tracks) == 0x28);

/*! A pitch range's permutations */
inline Span<sound_permutation_t const> permutations_of(
    sound_pitch_range_t const& range,
    sound_gestalt_t const&     gestalt,
    tag_magic const&           magic)
{
    auto all = gestalt.permutations.data(magic).value_or(
        Span<sound_permutation_t const>());
    size_t first = range.first_permutation(), count = range.permutation_count();
    if(first + count > all.size())
        return {};
    return all.subspan(first, count);
}

/*!
 * \brief A permutation's encoded bytes, its chunks joined. Chunk offsets
 * address the resource's primary bytes followed by its secondary bytes;
 * empty if a chunk falls outside them.
 */
std::vector<u8> permutation_data(
    sound_permutation_t const& permutation,
    sound_gestalt_t const&     gestalt,
    tag_magic const&           magic,
    resource_view const&       view);

/*!
 * \brief XMA2WAVEFORMATEX, little-endian as RIFF files and libavcodec's
 * xma2 decoder (as extradata past the WAVEFORMATEX head) expect. Every two
 * channels make one stream.
 */
struct xma2_format_t
{
    u16 format_tag{0x166};
    u16 channels{0};
    u32 samples_per_sec{0};
    u32 avg_bytes_per_sec{0};
    u16 block_align{0};
    u16 bits_per_sample{16};
    u16 extra_size{34};
    u16 num_streams{0};
    u32 channel_mask{0};
    u32 samples_encoded{0};
    u32 bytes_per_block{0};
    u32 play_begin{0};
    u32 play_length{0};
    u32 loop_begin{0};
    u32 loop_length{0};
    u8  loop_count{0};
    u8  encoder_version{4};
    u16 block_count{1};

    static xma2_format_t from(
        sound_codec_t const& codec, u32 samples, u32 bytes);
};

static_assert(sizeof(xma2_format_t) == 52);

} // namespace blam::elegy
