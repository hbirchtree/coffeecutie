#pragma once

/* Sound for Xbox maps: permutations stream out of the mapped map as they
 * play, read into the map's page cache without stopping the game for the disc,
 * and are decoded and mixed on the CPU for the audio DMA. Only the DMA hand-off
 * runs in the interrupt; mixing is done from frame stages, ahead by a few
 * blocks to ride out a slow frame. */

#include <blam/common/endian.h>
#include <blam/volta/blam_sound.h>
#include <blam/volta/blam_stl.h>

#include <platforms/gekko/paged_mmap.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <ogc/audio.h>
#include <ogc/cache.h>

#include "sound_paths.h"

#include <algorithm>
#include <cstring>
#include <map>
#include <optional>
#include <vector>

namespace gx_audio {

using blam::from_le;
using libc_types::f32;
using libc_types::i16;
using libc_types::i32;
using libc_types::u16;
using libc_types::u32;
using libc_types::u8;

constexpr u32 kAdpcmBlock   = 36; // per channel
constexpr u32 kAdpcmSamples = 65; // the header's sample, then 64 nibbles

/* The published IMA ADPCM step, for a block's worth of one channel. Blocks
 * lead with a header per channel; the channels' nibbles alternate by the
 * 32-bit word, low nibble first. `out` is filled from out[0] */
inline void decode_adpcm_block(
    u8 const* block, u32 channels, u32 channel, i16* out, u32 stride = 1)
{
    static constexpr i16 steps[89] = {
        7,     8,     9,     10,    11,    12,    13,    14,    16,    17,
        19,    21,    23,    25,    28,    31,    34,    37,    41,    45,
        50,    55,    60,    66,    73,    80,    88,    97,    107,   118,
        130,   143,   157,   173,   190,   209,   230,   253,   279,   307,
        337,   371,   408,   449,   494,   544,   598,   658,   724,   796,
        876,   963,   1060,  1166,  1282,  1411,  1552,  1707,  1878,  2066,
        2272,  2499,  2749,  3024,  3327,  3660,  4026,  4428,  4871,  5358,
        5894,  6484,  7132,  7845,  8630,  9493,  10442, 11487, 12635, 13899,
        15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767};
    static constexpr i32 adjust[8] = {-1, -1, -1, -1, 2, 4, 6, 8};

    u8 const* header    = block + channel * 4;
    i32       predictor = static_cast<i16>(header[0] | (header[1] << 8));
    i32       index     = std::clamp<i32>(header[2], 0, 88);
    *out                = static_cast<i16>(predictor);
    out += stride;

    u8 const* words = block + channels * 4;
    for(u32 word = 0; word < 8; word++)
    {
        u8 const* bytes = words + (word * channels + channel) * 4;
        for(u32 n = 0; n < 8; n++)
        {
            u32 const nibble = (bytes[n / 2] >> ((n & 1) * 4)) & 0xF;
            i32 const step   = steps[index];
            i32       diff   = step >> 3;
            if(nibble & 1)
                diff += step >> 2;
            if(nibble & 2)
                diff += step >> 1;
            if(nibble & 4)
                diff += step;
            predictor += nibble & 8 ? -diff : diff;
            predictor = std::clamp(predictor, -32768, 32767);
            index     = std::clamp(index + adjust[nibble & 7], 0, 88);
            *out      = static_cast<i16>(predictor);
            out += stride;
        }
    }
}

struct permutation_t
{
    u32 file_off{0}, size{0};
    f32 gain{1.f};
    i16 next{-1}; // follow-on chunk of this piece, relative to the sound
};

struct sound_t
{
    u32       first{0}, count{0}; // in bank_t::permutations
    u32       starts{0};          // the first few begin a piece
    u32       rate{22050};
    u32       channels{1};
    f32       near{1.f}, far{30.f};
    f32       gain{1.f};
    glm::vec2 pitch{1.f};
};

/* A looping sound's tracks and details, as a cluster's background */
struct background_t
{
    struct track_t
    {
        u32 sound;
        f32 gain, fade_in, fade_out;
    };

    struct detail_t
    {
        u32       sound;
        glm::vec2 period, yaw, pitch, distance;
        f32       gain;
    };

    std::vector<track_t>  tracks;
    std::vector<detail_t> details;
};

/* The sounds something can play, read out of the map at load */
class bank_t
{
  public:
    std::vector<sound_t>       sounds;
    std::vector<permutation_t> permutations;
    bool                       enabled{false};

    template<typename V, typename Ref>
    std::optional<u32> sound(blam::map_container<V> const& c, Ref const& ref)
    {
        using blam::sound::sound;
        auto const* t = enabled ? blam::tag_at(c, ref.tag_id) : nullptr;
        if(!t || !t->matches(blam::tag_class_t::snd))
            return std::nullopt;
        if(auto it = m_known.find(ref.tag_id.value()); it != m_known.end())
            return it->second;
        auto tag = t->template data<sound>(c.magic);
        if(tag.has_error())
            return std::nullopt;
        auto const& s = *tag.value();
        if(from_le(s.codec) != sound::xbox_adpcm)
            return std::nullopt;

        blam::tag_index_view<V> index(c);
        index.add_atlas(blam::atlas_type_t::sounds, c.magic);
        auto ranges = index.deref(*t, s.pitch_ranges_);
        if(!ranges.has_value() || ranges.value().empty())
            return std::nullopt;
        auto const& range = ranges.value()[0];
        auto        perms = index.deref(*t, range.permutations_);
        if(!perms.has_value() || perms.value().empty())
            return std::nullopt;

        sound_t out{
            .first  = static_cast<u32>(permutations.size()),
            .count  = static_cast<u32>(perms.value().size()),
            .starts = std::clamp<u32>(
                static_cast<u16>(from_le(range.actual_permutation_count)),
                1,
                static_cast<u32>(perms.value().size())),
            .rate = from_le(s.sample_rate) == sound::_44kHz ? 44100u : 22050u,
            .channels = from_le(s.channels) == sound::stereo ? 2u : 1u,
            .near     = from_le(s.min_distance),
            .far      = from_le(s.max_distance),
            .gain     = from_le(s.gain_modifier),
            .pitch =
                {from_le(s.random_pitch_bounds.x),
                 from_le(s.random_pitch_bounds.y)},
        };
        // Distances of 0 take the sound class's own
        auto const defaults = blam::sound::default_distances(from_le(s.type));
        if(out.near <= 0.f)
            out.near = defaults.x > 0.f ? defaults.x : 1.f;
        if(out.far <= 0.f)
            out.far = defaults.y > 0.f ? defaults.y : out.near * 30.f;
        if(out.gain <= 0.f)
            out.gain = 1.f;
        if(out.pitch.x <= 0.f || out.pitch.y <= 0.f)
            out.pitch = glm::vec2{1.f};
        for(auto const& p : perms.value())
            permutations.push_back({
                .file_off = from_le(p.sample_offset),
                .size     = from_le(p.sample_size),
                .gain     = from_le(p.gain),
                .next     = from_le(p.next_permutation_idx),
            });
        sounds.push_back(out);
        return m_known[ref.tag_id.value()] =
                   static_cast<u32>(sounds.size() - 1);
    }

    /* A looping sound: its tracks' loops, and the details heard around them.
     * Alternate loops wait on scripts, so details only for those are left out
     */
    template<typename V, typename Ref>
    std::optional<background_t> background(
        blam::map_container<V> const& c, Ref const& ref)
    {
        using blam::sound::detail_sound_t;
        using blam::sound::looping_sound;
        auto const* t = enabled ? blam::tag_at(c, ref.tag_id) : nullptr;
        if(!t || !t->matches(blam::tag_class_t::lsnd))
            return std::nullopt;
        auto tag = t->template data<looping_sound>(c.magic);
        if(tag.has_error())
            return std::nullopt;
        background_t out;
        if(auto tracks = tag.value()->tracks.data(c.magic); !tracks.has_error())
            for(auto const& tr : tracks.value())
                if(auto s = sound(c, tr.loop))
                    out.tracks.push_back({
                        .sound    = *s,
                        .gain     = from_le(tr.gain),
                        .fade_in  = from_le(tr.fade_in_duration),
                        .fade_out = from_le(tr.fade_out_duration),
                    });
        if(auto details = tag.value()->detail_sounds.data(c.magic);
           !details.has_error())
            for(auto const& d : details.value())
            {
                if(from_le(d.flags) &
                   detail_sound_t::dont_play_without_alternate)
                    continue;
                auto const s = sound(c, d.sound);
                if(!s)
                    continue;
                auto le2 = [](typing::vector_types::Vecf2 const& v) {
                    return glm::vec2{from_le(v.x), from_le(v.y)};
                };
                out.details.push_back({
                    .sound    = *s,
                    .period   = le2(d.random_period_bounds),
                    .yaw      = le2(d.yaw_bounds),
                    .pitch    = le2(d.pitch_bounds),
                    .distance = le2(d.distance_bounds),
                    .gain     = from_le(d.gain),
                });
            }
        if(out.tracks.empty() && out.details.empty())
            return std::nullopt;
        return out;
    }

  private:
    std::map<u32, u32> m_known; // by tag id
};

class mixer_t
{
  public:
    explicit mixer_t(bank_t const& bank)
        : m_bank(bank)
    {
    }

    /* map: the mapped map file, which has to stay mapped while sounds play */
    void begin(platform::file::gekko::vmem::mapping map, paths_t const& paths)
    {
        s_self  = this;
        m_map   = map;
        m_paths = &paths;
        for(u32 i = 0; i < kAmbient; i++)
            for(u32 h = 0; h < kParts; h++)
                m_ambient[i].store[h] = m_ambient_store[i][h];
        std::memset(m_blocks, 0, sizeof(m_blocks));
        std::memset(m_silence, 0, sizeof(m_silence));
        DCFlushRange(m_blocks, sizeof(m_blocks));
        DCFlushRange(m_silence, sizeof(m_silence));
        AUDIO_Init(nullptr);
        AUDIO_SetDSPSampleRate(AI_SAMPLERATE_32KHZ);
        AUDIO_RegisterDMACallback(&mixer_t::on_dma);
        AUDIO_InitDMA(reinterpret_cast<u32>(m_blocks[0]), kBlockBytes);
        AUDIO_StartDMA();
    }

    /* Where the sounds are heard from; `right` is the ear's axis. One voice
     * a frame looks again at the structure around it */
    void listen(glm::vec3 const& at, glm::vec3 const& right, f32 dt)
    {
        m_ear   = at;
        m_right = right;
        for(u32 i = 0; i < kVoices; i++)
        {
            auto& v = m_voices[(m_sense_next + i) % kVoices];
            if(v.active)
            {
                sense(v);
                m_sense_next = (m_sense_next + i + 1) % kVoices;
                break;
            }
        }
        f32 const blend = std::min(1.f, dt / kOcclusionSmoothing);
        for(auto& v : m_voices)
            if(v.active)
            {
                v.occlusion += (v.occlusion_target - v.occlusion) * blend;
                spatialize(v);
            }
        for(auto& v : m_ambient)
            if(v.active)
            {
                v.fade   = std::clamp(v.fade + v.fade_rate * dt, 0.f, 1.f);
                v.active = v.fade > 0.f || v.fade_rate > 0.f;
                i32 const volume =
                    static_cast<i32>(v.gain * v.perm_gain * v.fade * 256.f);
                v.volume[0] = v.volume[1] = volume;
            }
        play_details(dt);
    }

    /* The cluster's background: the last one's tracks fade out as these fade
     * in. Null keeps what plays */
    void set_background(background_t const* background)
    {
        if(!background || background == m_background)
            return;
        for(auto& v : m_ambient)
            if(v.active && v.fade_rate >= 0.f)
                v.fade_rate = -1.f / std::max(v.fade_out, .1f);
        m_background = background;
        m_detail_timers.clear();
        for(auto const& d : background->details)
            m_detail_timers.push_back(uniform(d.period));
        for(auto const& track : background->tracks)
            start_ambient(track);
    }

    /* direct: heard as placed, with nothing in the way looked for */
    void play(
        u32 sound, glm::vec3 const& at, f32 gain = 1.f, bool direct = false)
    {
        if(sound >= m_bank.sounds.size())
            return;
        auto const& s        = m_bank.sounds[sound];
        u32 const   pick     = random() % s.starts;
        voice_t*    slot     = nullptr;
        u32         same     = 0;
        voice_t*    oldest   = nullptr;
        voice_t*    quietest = nullptr;
        for(auto& v : m_voices)
        {
            if(!v.active)
            {
                slot = slot ? slot : &v;
                continue;
            }
            if(v.sound == sound)
            {
                same++;
                if(!oldest || v.started < oldest->started)
                    oldest = &v;
            }
            if(!quietest || loudness(v) < loudness(*quietest))
                quietest = &v;
        }
        if(same >= kMaxSameSound)
            slot = oldest;
        if(!slot)
            slot = quietest;

        f32 const bend = glm::mix(
            s.pitch.x,
            s.pitch.y,
            static_cast<f32>(random() & 0xFFFF) / 65535.f);
        voice_t& v = *slot;
        v.active   = true;
        v.sound    = sound;
        v.channels = s.channels;
        v.at       = at;
        v.gain     = s.gain * gain;
        v.direct   = direct;
        v.started  = ++m_plays;
        v.step     = static_cast<u32>((s.rate * bend / kOutputRate) * 65536.f);
        v.pos      = kAdpcmSamples << 16; // decode on the first sample
        std::memset(v.pcm, 0, sizeof(v.pcm));
        start_permutation(v, s.first + pick);
        v.lowpassed[0] = v.lowpassed[1] = 0;
        sense(v);
        v.occlusion = v.occlusion_target;
        spatialize(v);
    }

    /* Mix whatever the DMA has room for */
    void pump()
    {
        if(auto done = platform::file::gekko::vmem::poll(); done.into)
            for(auto& v : m_ambient)
                for(u32 h = 0; h < kParts; h++)
                    if(v.store[h] == done.into && v.half[h].asked)
                    {
                        v.half[h].asked = false;
                        v.half[h].ready = done.ok;
                    }
        while(m_filled - m_playing < kBlocks - 2)
        {
            i16* block = m_blocks[m_filled % kBlocks];
            mix(block);
            DCFlushRange(block, kBlockBytes);
            m_filled = m_filled + 1;
        }
        request_reads();
    }

  private:
    static constexpr u32 kOutputRate   = 32000;
    static constexpr u32 kFrames       = 320; // 10 ms
    static constexpr u32 kBlockBytes   = kFrames * 4;
    static constexpr u32 kBlocks       = 8;
    static constexpr u32 kVoices       = 12;
    static constexpr u32 kMaxSameSound = 3;
    static constexpr u32 kReadAhead    = 8192; // bytes a voice keeps in front
    static constexpr u32 kAmbient      = 4;    // background tracks' own budget
    // One of an ambient voice's buffers: whole blocks of one or two channels.
    // Several, as a piece's last chunk can be tiny
    static constexpr u32 kParts     = 3;
    static constexpr u32 kHalfBytes = kAdpcmBlock * 2 * 112;
    static constexpr f32 kDefaultFade =
        2.f; // seconds, for tracks that give none
    static constexpr f32 kAmbientUrgent =
        .3f;                             // seconds left before it jumps ahead
    static constexpr i32 kHeadroom = 96; // 8.8: gunshots peak at full scale
    static constexpr f32 kOcclusionSmoothing = .3f; // seconds
    // What full occlusion costs, as BlamGraphics' dry path
    static constexpr f32 kOccludedGain = .65f, kOccludedHighs = .85f;

    struct voice_t
    {
        bool      active{false};
        u32       sound{0};
        u32       channels{1};
        glm::vec3 at{0.f};
        f32       gain{1.f}, perm_gain{1.f};
        u32       started{0};

        u32 step{0}; // source samples per output frame, 16.16
        u32 pos{0};  // in pcm, 16.16

        i32  volume[2]{0, 0};     // 8.8
        i16  next{-1};            // follow-on permutation
        u32  at_file{0}, left{0}; // compressed bytes still to come
        bool waiting{false};      // for its next block to come off the disc

        f32       occlusion{0.f}, occlusion_target{0.f};
        bool      routed{false};
        glm::vec3 route_through{0.f};
        f32       route_length{0.f};
        i32       lowpass{256}; // 8.8, of the difference taken each sample
        i32       lowpassed[2]{0, 0};

        bool direct{false}; // nothing in the way looked for

        // Background tracks: their own buffers, loop on, fade in and out
        bool ambient{false};
        f32  fade{1.f}, fade_rate{0.f}, fade_out{kDefaultFade};

        struct half_t
        {
            u32  perm{0}, at{0}, len{0}, skip{0};
            bool planned{false}, asked{false}, ready{false};
        };

        half_t half[kParts];
        u8*    store[kParts]{};
        u32    reading{0}, used{0};
        u32    plan_perm{0}, plan_at{0}; // where the next half to read starts

        // [0] repeats the last block's end, for interpolating over the seam
        i16 pcm[2][kAdpcmSamples + 1];
    };

    static inline mixer_t* s_self = nullptr;

    /* Runs in the interrupt: no floats, no touching voices */
    static void on_dma()
    {
        auto&     m    = *s_self;
        u32 const next = m.m_playing + 1;
        if(next == m.m_filled)
        {
            AUDIO_InitDMA(reinterpret_cast<u32>(m.m_silence), kBlockBytes);
            return;
        }
        m.m_playing = next;
        AUDIO_InitDMA(
            reinterpret_cast<u32>(m.m_blocks[next % kBlocks]), kBlockBytes);
    }

    u32 random()
    {
        m_seed ^= m_seed << 13;
        m_seed ^= m_seed >> 17;
        m_seed ^= m_seed << 5;
        return m_seed;
    }

    static i32 loudness(voice_t const& v)
    {
        return v.volume[0] + v.volume[1];
    }

    /* What stands between this voice and the ear */
    void sense(voice_t& v)
    {
        v.occlusion_target = 0.f;
        v.routed           = false;
        if(v.direct || !m_paths || !m_paths->hears_from(m_ear))
            return;
        auto const open = m_paths->open_near(v.at, m_ear);
        if(!open)
            return;
        v.occlusion_target = m_paths->occlusion(m_ear, *open);
        if(v.occlusion_target <= 0.f)
            return;
        if(auto const way = m_paths->route(*open, m_ear))
        {
            v.routed        = true;
            v.route_through = way->through;
            v.route_length  = way->length;
        }
    }

    void spatialize(voice_t& v)
    {
        auto const& s        = m_bank.sounds[v.sound];
        glm::vec3   to       = v.at - m_ear;
        f32         distance = glm::length(to);
        // The more it's blocked, the more it comes the way round it can
        if(v.routed && distance > 1e-3f)
        {
            glm::vec3 const by    = v.route_through - m_ear;
            f32 const       reach = glm::length(by);
            if(reach > 1e-3f)
            {
                to       = glm::mix(to / distance, by / reach, v.occlusion);
                distance = glm::mix(distance, v.route_length, v.occlusion);
            }
        }
        f32 gain = distance <= s.near  ? 1.f
                   : distance >= s.far ? 0.f
                                       : s.near / distance;
        gain *= v.gain * v.perm_gain *
                (1.f - kOccludedGain * v.occlusion * v.occlusion);
        v.lowpass =
            static_cast<i32>((1.f - kOccludedHighs * v.occlusion) * 256.f);
        // Panned by how far off to the side it is
        f32 const span = glm::length(to);
        f32 const pan =
            span > 1e-3f && v.channels == 1
                ? std::clamp(glm::dot(to / span, m_right), -1.f, 1.f)
                : 0.f;
        v.volume[0] =
            static_cast<i32>(gain * (1.f - std::max(pan, 0.f) * .7f) * 256.f);
        v.volume[1] =
            static_cast<i32>(gain * (1.f + std::min(pan, 0.f) * .7f) * 256.f);
    }

    void start_permutation(voice_t& v, u32 index)
    {
        auto const& p = m_bank.permutations[index];
        v.at_file     = p.file_off;
        v.left        = p.size;
        v.perm_gain   = p.gain > 0.f ? p.gain : 1.f;
        v.next        = p.next;
    }

    f32 uniform(glm::vec2 const& bounds)
    {
        return glm::mix(
            bounds.x, bounds.y, static_cast<f32>(random() & 0xFFFF) / 65535.f);
    }

    /* Each detail at its own random period, around the listener; yaw from
     * where they face */
    void play_details(f32 dt)
    {
        if(!m_background)
            return;
        glm::vec3 const up{0.f, 0.f, 1.f};
        glm::vec3 const forward = glm::normalize(glm::cross(up, m_right));
        for(size_t i = 0; i < m_detail_timers.size(); i++)
        {
            if((m_detail_timers[i] -= dt) > 0.f)
                continue;
            auto const& d       = m_background->details[i];
            m_detail_timers[i]  = std::max(uniform(d.period), .5f);
            f32 const       yaw = uniform(d.yaw), pitch = uniform(d.pitch);
            glm::vec3 const offset =
                uniform(d.distance) *
                (forward * (std::cos(pitch) * std::cos(yaw)) +
                 m_right * (std::cos(pitch) * std::sin(yaw)) +
                 up * std::sin(pitch));
            play(d.sound, m_ear + offset, d.gain, true);
        }
    }

    void start_ambient(background_t::track_t const& track)
    {
        // A free one, else whichever has faded furthest
        voice_t* slot = &m_ambient[0];
        for(auto& v : m_ambient)
            if(!v.active ? slot->active || v.fade < slot->fade
                         : slot->active && v.fade < slot->fade)
                slot = &v;
        auto const& s = m_bank.sounds[track.sound];
        voice_t&    v = *slot;
        u8*         store[kParts];
        std::copy(v.store, v.store + kParts, store);
        v = {};
        std::copy(store, store + kParts, v.store);
        v.active   = true;
        v.ambient  = true;
        v.sound    = track.sound;
        v.channels = s.channels;
        v.gain     = s.gain * (track.gain > 0.f ? track.gain : 1.f);
        v.fade     = 0.f;
        v.fade_rate =
            1.f / (track.fade_in > 0.f ? track.fade_in : kDefaultFade);
        v.fade_out  = track.fade_out > 0.f ? track.fade_out : kDefaultFade;
        v.step      = static_cast<u32>((s.rate / f32(kOutputRate)) * 65536.f);
        v.pos       = kAdpcmSamples << 16;
        v.plan_perm = random() % s.starts;
    }

    /* The next stretch of a background track: on through its chunks, then
     * another piece to start, as loops go on */
    void plan(voice_t& v, voice_t::half_t& h)
    {
        auto const& s     = m_bank.sounds[v.sound];
        u32 const   block = kAdpcmBlock * v.channels;
        for(u32 guard = 0; guard < 4; guard++)
        {
            auto const& p = m_bank.permutations[s.first + v.plan_perm];
            if(v.plan_at + block <= p.size)
            {
                u32 const len =
                    std::min(kHalfBytes, (p.size - v.plan_at) / block * block);
                h         = {.perm = v.plan_perm, .at = v.plan_at, .len = len};
                h.planned = true;
                v.plan_at += len;
                return;
            }
            v.plan_at   = 0;
            v.plan_perm = p.next >= 0 && static_cast<u32>(p.next) < s.count
                              ? static_cast<u32>(p.next)
                              : random() % s.starts;
        }
    }

    /* The first byte of what the voice needs next not yet off the disc */
    std::optional<u32> missing(voice_t const& v, u32 span) const
    {
        namespace vmem  = platform::file::gekko::vmem;
        u32 const here  = std::min(v.left, span);
        auto      first = vmem::first_missing(m_map, v.at_file, here);
        if(first || here == span || v.next < 0)
            return first;
        auto const& s = m_bank.sounds[v.sound];
        if(static_cast<u32>(v.next) >= s.count)
            return first;
        auto const& p = m_bank.permutations[s.first + v.next];
        return vmem::first_missing(
            m_map, p.file_off, std::min(p.size, span - here));
    }

    /* Seconds a background track has read and not yet played */
    f32 ambient_left(voice_t const& v) const
    {
        u32 const block = kAdpcmBlock * v.channels;
        u32       bytes = 0;
        for(u32 n = 0; n < kParts; n++)
        {
            auto const& h = v.half[(v.reading + n) % kParts];
            if(!h.ready)
                break;
            bytes += h.len - (n ? 0 : v.used);
        }
        return static_cast<f32>(bytes / block * kAdpcmSamples) /
               static_cast<f32>(m_bank.sounds[v.sound].rate);
    }

    /* Refill a background track's next empty half; false if the drive's busy
     * or there's nothing to read */
    bool read_ambient(voice_t& v)
    {
        namespace vmem = platform::file::gekko::vmem;
        for(u32 n = 0; n < kParts; n++)
        {
            u32 const i = (v.reading + n) % kParts;
            auto&     h = v.half[i];
            if(!v.active || h.ready || h.asked)
                continue;
            if(!h.planned)
                plan(v, h);
            if(!h.planned)
                continue;
            auto const& s = m_bank.sounds[v.sound];
            if(!vmem::read(
                   m_map,
                   m_bank.permutations[s.first + h.perm].file_off + h.at,
                   h.len,
                   v.store[i],
                   h.skip))
                return false;
            h.asked = true;
            return true;
        }
        return false;
    }

    /* One read at a time: voices run dry first, then background tracks about
     * to, then reading ahead, then background tracks with time to spare */
    void request_reads()
    {
        namespace vmem = platform::file::gekko::vmem;
        auto voices    = [&](u32 span) {
            for(auto const& v : m_voices)
                if(v.active && (span == kReadAhead || v.waiting))
                    if(auto at = missing(v, span))
                    {
                        vmem::request(m_map, *at);
                        return true;
                    }
            return false;
        };
        if(voices(kAdpcmBlock * 2))
            return;
        for(auto& v : m_ambient)
            if(v.active && ambient_left(v) < kAmbientUrgent && read_ambient(v))
                return;
        if(voices(kReadAhead))
            return;
        for(auto& v : m_ambient)
            if(read_ambient(v))
                return;
    }

    enum class block_t
    {
        decoded,
        waiting, // not off the disc yet
        over,
    };

    /* The next block's samples into pcm */
    block_t next_block(voice_t& v)
    {
        u32 const block = kAdpcmBlock * v.channels;
        if(v.ambient)
            return next_ambient_block(v, block);
        while(v.left < block)
        {
            if(v.next < 0)
                return block_t::over;
            auto const& s = m_bank.sounds[v.sound];
            if(static_cast<u32>(v.next) >= s.count)
                return block_t::over;
            start_permutation(v, s.first + v.next);
        }
        v.waiting =
            platform::file::gekko::vmem::first_missing(m_map, v.at_file, block)
                .has_value();
        if(v.waiting)
            return block_t::waiting;
        for(u32 c = 0; c < v.channels; c++)
        {
            v.pcm[c][0] = v.pcm[c][kAdpcmSamples];
            decode_adpcm_block(
                m_map.base + v.at_file, v.channels, c, &v.pcm[c][1]);
        }
        v.at_file += block;
        v.left -= block;
        return block_t::decoded;
    }

    /* From the half being played; a spent half goes back to be refilled */
    block_t next_ambient_block(voice_t& v, u32 block)
    {
        auto* h = &v.half[v.reading];
        if(h->ready && v.used + block > h->len)
        {
            *h        = {};
            v.reading = (v.reading + 1) % kParts;
            v.used    = 0;
            h         = &v.half[v.reading];
        }
        if(!h->ready)
            return block_t::waiting;
        auto const& s = m_bank.sounds[v.sound];
        v.perm_gain =
            std::max(m_bank.permutations[s.first + h->perm].gain, 0.f);
        if(v.perm_gain <= 0.f)
            v.perm_gain = 1.f;
        u8 const* at = v.store[v.reading] + h->skip + v.used;
        for(u32 c = 0; c < v.channels; c++)
        {
            v.pcm[c][0] = v.pcm[c][kAdpcmSamples];
            decode_adpcm_block(at, v.channels, c, &v.pcm[c][1]);
        }
        v.used += block;
        return block_t::decoded;
    }

    void mix(i16* out)
    {
        std::memset(m_mix, 0, sizeof(m_mix));
        for(auto& v : m_voices)
            mix_voice(v);
        for(auto& v : m_ambient)
            mix_voice(v);
        for(u32 s = 0; s < kFrames * 2; s++)
            out[s] = static_cast<i16>(
                std::clamp((m_mix[s] * kHeadroom) >> 8, -32768, 32767));
    }

    void mix_voice(voice_t& v)
    {
        {
            if(!v.active)
                return;
            u32 const other = v.channels - 1;
            for(u32 f = 0; f < kFrames; f++)
            {
                if(v.pos >= kAdpcmSamples << 16)
                {
                    auto const got = next_block(v);
                    if(got == block_t::over)
                        v.active = false;
                    if(got != block_t::decoded)
                        break;
                    v.pos -= kAdpcmSamples << 16;
                }
                u32 const i    = v.pos >> 16;
                i32 const frac = static_cast<i32>((v.pos & 0xFFFF) >> 1);
                auto      at   = [&](u32 c) {
                    i32 const a = v.pcm[c][i], b = v.pcm[c][i + 1];
                    i32 const sample = a + (((b - a) * frac) >> 15);
                    v.lowpassed[c] +=
                        ((sample - v.lowpassed[c]) * v.lowpass) >> 8;
                    return v.lowpassed[c];
                };
                i32 const left  = at(0);
                i32 const right = other ? at(other) : left;
                // The DMA takes right before left
                m_mix[f * 2 + 1] += (left * v.volume[0]) >> 8;
                m_mix[f * 2] += (right * v.volume[1]) >> 8;
                v.pos += v.step;
            }
        }
    }

    bank_t const&                        m_bank;
    platform::file::gekko::vmem::mapping m_map;
    paths_t const*                       m_paths{nullptr};
    u32                                  m_sense_next{0};
    voice_t                              m_voices[kVoices];
    voice_t                              m_ambient[kAmbient];

    background_t const* m_background{nullptr};
    std::vector<f32>    m_detail_timers;
    u8  m_ambient_store[kAmbient][kParts][kHalfBytes + 64] ATTRIBUTE_ALIGN(32);
    i32 m_mix[kFrames * 2];
    glm::vec3 m_ear{0.f}, m_right{0.f, -1.f, 0.f};
    u32       m_seed{0x2F6E2B1u};
    u32       m_plays{0};

    i16 m_blocks[kBlocks][kFrames * 2] ATTRIBUTE_ALIGN(32);
    i16 m_silence[kFrames * 2] ATTRIBUTE_ALIGN(32);

    u32 volatile m_playing{0};
    u32 volatile m_filled{1};
};

} // namespace gx_audio
