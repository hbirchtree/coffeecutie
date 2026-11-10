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
    void begin(platform::file::gekko::vmem::mapping map)
    {
        s_self = this;
        m_map  = map;
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

    /* Where the sounds are heard from; `right` is the ear's axis */
    void listen(glm::vec3 const& at, glm::vec3 const& right)
    {
        m_ear   = at;
        m_right = right;
        for(auto& v : m_voices)
            if(v.active)
                spatialize(v);
    }

    void play(u32 sound, glm::vec3 const& at)
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
        v.gain     = s.gain;
        v.started  = ++m_plays;
        v.step     = static_cast<u32>((s.rate * bend / kOutputRate) * 65536.f);
        v.pos      = kAdpcmSamples << 16; // decode on the first sample
        std::memset(v.pcm, 0, sizeof(v.pcm));
        start_permutation(v, s.first + pick);
        spatialize(v);
    }

    /* Mix whatever the DMA has room for */
    void pump()
    {
        platform::file::gekko::vmem::poll();
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
    static constexpr i32 kHeadroom     = 96; // 8.8: gunshots peak at full scale

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

    void spatialize(voice_t& v)
    {
        auto const&     s        = m_bank.sounds[v.sound];
        glm::vec3 const to       = v.at - m_ear;
        f32 const       distance = glm::length(to);
        f32             gain     = distance <= s.near  ? 1.f
                                   : distance >= s.far ? 0.f
                                                       : s.near / distance;
        gain *= v.gain * v.perm_gain;
        // Panned by how far off to the side it is
        f32 const pan =
            distance > 1e-3f && v.channels == 1
                ? std::clamp(glm::dot(to / distance, m_right), -1.f, 1.f)
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

    /* One read at a time: voices run dry first, then those about to */
    void request_reads()
    {
        namespace vmem = platform::file::gekko::vmem;
        for(u32 span : {kAdpcmBlock * 2, kReadAhead})
            for(auto const& v : m_voices)
                if(v.active && (span == kReadAhead || v.waiting))
                    if(auto at = missing(v, span))
                    {
                        vmem::request(m_map, *at);
                        return;
                    }
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

    void mix(i16* out)
    {
        std::memset(m_mix, 0, sizeof(m_mix));
        for(auto& v : m_voices)
        {
            if(!v.active)
                continue;
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
                    return a + (((b - a) * frac) >> 15);
                };
                // The DMA takes right before left
                m_mix[f * 2 + 1] += (at(0) * v.volume[0]) >> 8;
                m_mix[f * 2] += (at(other) * v.volume[1]) >> 8;
                v.pos += v.step;
            }
        }
        for(u32 s = 0; s < kFrames * 2; s++)
            out[s] = static_cast<i16>(
                std::clamp((m_mix[s] * kHeadroom) >> 8, -32768, 32767));
    }

    bank_t const&                        m_bank;
    platform::file::gekko::vmem::mapping m_map;
    voice_t                              m_voices[kVoices];
    i32                                  m_mix[kFrames * 2];
    glm::vec3                            m_ear{0.f}, m_right{0.f, -1.f, 0.f};
    u32                                  m_seed{0x2F6E2B1u};
    u32                                  m_plays{0};

    i16 m_blocks[kBlocks][kFrames * 2] ATTRIBUTE_ALIGN(32);
    i16 m_silence[kFrames * 2] ATTRIBUTE_ALIGN(32);

    u32 volatile m_playing{0};
    u32 volatile m_filled{1};
};

} // namespace gx_audio
