#pragma once

#include "caching_item.h"
#include "data_cache.h"
#include "peripherals/concepts/sound_api.h"
#include <blam/volta/blam_sound.h>
#include <blam/volta/blam_stl.h>
#include <coffee/components/subsystem.h>

#if defined(FEATURE_ENABLE_OAF)
#include <oaf/api.h>
#include <oaf/ogg/ogg_decode.h>
#if defined(OAF_IMA_DECODER_ENABLED)
#include <oaf/ima_adpcm/decode.h>
#endif
#else
namespace oaf {
struct api;
struct buffer_t;
}
#endif

using sound_ptr =
    std::tuple<blam::tag_t const*, blam::sound::sound const*, blam::map_ptr>;

struct SoundItem
{
    enum role_t
    {
        start,
        end,
        alt_end,
        loop,
        alt_loop,
        main,
    };

    struct permutation_t
    {
        blam::sound::pitch_permutation_t const* permutation{nullptr};
        std::shared_ptr<oaf::buffer_t> buffer;
    };

    struct pitch_range_t
    {
        blam::sound::pitch_range_t const* range{nullptr};
        std::vector<permutation_t> permutations;
    };

    struct track_t
    {
        blam::sound::track_t const* track{nullptr};
        std::map<role_t, sound_ptr> sounds;
        std::map<role_t, std::vector<pitch_range_t>> buffers;
    };

    blam::sound::looping_sound const* looping_sound{nullptr};
    std::vector<track_t> tracks;
    std::vector<track_t> detail_sounds;

    bool single() const
    {
        if(tracks.size() == 1)
            return tracks[0].sounds.size() == 1;
        return false;
    }

    bool valid() const
    {
        return !tracks.empty();
    }
};

template<typename V>
struct SoundCache
    : DataCache<SoundItem, u32, blam::tagref_t const&>
    , compo::SubsystemBase
{
    using type = SoundCache<V>;

    SoundCache(oaf::api* api)
        : api(api)
    {
    }

    inline void load_from(blam::map_container<V> const& map)
    {
        index = blam::tag_index_view<V>(map);
        if(map.map->version == blam::version_t::xbox)
            index.add_atlas(blam::atlas_type_t::sounds, map.magic);
        evict_all();
    }

    inline void load_sounds_from(blam::map_ptr const& sound_ptr)
    {
        index.add_atlas(blam::atlas_type_t::sounds, sound_ptr);
    }

    oaf::api*               api{nullptr};
    blam::tag_index_view<V> index;

#if defined(FEATURE_ENABLE_OAF)
    void upload_singular_samples(
        std::vector<SoundItem::pitch_range_t>& ranges,
        sound_ptr const& sound_)
    {
        auto [tag, sound, heap] = sound_;
        auto ranges_ = *index.deref(*tag, sound->pitch_ranges_);
        if(ranges_.empty())
            return;
        for(auto const& range : ranges_)
        {
            cDebug("- Range");
            ranges.emplace_back(SoundItem::pitch_range_t{
                .range = &range,
            });
            auto perms_ = index.deref(*tag, range.permutations_);
            if(!perms_.has_value())
                continue;
            auto& out = ranges.back();
            for(auto const& perm : perms_.value())
            {
                cDebug("  - Permutation");
                out.permutations.emplace_back(SoundItem::permutation_t{
                    .permutation = &perm,
                });
                auto data_ = index.deref(*tag, perm.sample_data());
                if(!data_.has_value())
                    continue;
                auto data = data_.value();
                auto& buffer = out.permutations.back().buffer;
                using codec_t = blam::sound::sound::codec_t;
                switch(sound->codec)
                {
                case codec_t::ima_adpcm:
                case codec_t::xbox_adpcm: {
                    oaf::format_t fmt;
                    fmt.format = oaf::Format::ima_adpcm;
                    fmt.frequency =
                        sound->sample_rate == blam::sound::sound::_22kHz
                            ? 22050 
                            : 44100;
                    fmt.channels =
                        sound->channels == blam::sound::sound::stereo
                            ? 2
                            : 1;
                    buffer = api->alloc_buffer();
                    if(api->formats().ima4_adpcm)
                    {
                        buffer->upload(data, fmt);
                    }
#if defined(OAF_IMA_DECODER_ENABLED)
                    else
                    {
                        oaf::decode::ima_adpcm::decoder decoder;
                        decoder.decode(data, fmt, *buffer);
                    }
#endif
                    break;
                }
                case codec_t::ogg: {
                    oaf::decode::ogg::decoder decoder;
                    decoder.decode(data, {}, {}, *buffer);
                    break;
                }
                case codec_t::pcm: {
                    cWarning("Implement PCM loading");
                    break;
                }
                }
            }
        }
    }

    void upload_samples(SoundItem& item)
    {
        for(auto& track : item.tracks)
            for(auto& [role, sound] : track.sounds)
            {
                auto it = track.buffers.emplace(
                    role,
                    std::vector<SoundItem::pitch_range_t>{});
                upload_singular_samples(it.first->second, sound);
            }
    }

    sound_ptr parse_simple_sound(blam::tagref_t const& tag)
    {
        auto data = index.template resource<blam::sound::sound>(tag);
        if(!data.has_value())
            return {};
        return *data;
    }

    void parse_loop_sound(SoundItem& item, blam::tagref_t const& tag)
    {
        auto it = index.template data<blam::sound::looping_sound>(tag);
        if(!it.has_value())
            return;

        auto const* lsnd   = it.value();
        item.looping_sound = lsnd;

        auto tracks = index.deref(lsnd->tracks).value();
        for(blam::sound::track_t const& track : tracks)
        {
            item.tracks.push_back({
                .track = &track,
            });
            auto& sounds             = item.tracks.back().sounds;
            sounds[SoundItem::start] = parse_simple_sound(track.start);
            sounds[SoundItem::loop]  = parse_simple_sound(track.loop);
            sounds[SoundItem::end]   = parse_simple_sound(track.end);
            std::erase_if(sounds, [](auto const& p) {
                return std::get<1>(p.second) == nullptr;
            });
        }
        auto detail_sounds = index.deref(lsnd->detail_sounds).value();
        for(blam::sound::detail_sound_t const& d_sound : detail_sounds)
        {
            cDebug("Detail sound");
            item.detail_sounds.push_back({
                .sounds = {
                    {SoundItem::main, parse_simple_sound(d_sound.sound)},
                },
            });
        }
    }
#endif

    SoundItem predict_impl(blam::tagref_t const& tag)
    {
#if defined(FEATURE_ENABLE_OAF)
        if(!tag.matches(blam::tag_class_t::snd) &&
           !tag.matches(blam::tag_class_t::lsnd))
            return {};
        SoundItem out;

        cDebug("Sound:");
        switch(tag.tag_class)
        {
        case blam::tag_class_t::lsnd:
            parse_loop_sound(out, tag);
            break;
        case blam::tag_class_t::snd:
            out.tracks.push_back({
                    .sounds = {{SoundItem::main, parse_simple_sound(tag)}}
            });
            break;
        default:
            break;
        }

        // TODO: Make smartness about discarding unplayed buffers
        cDebug("Sound:");
        upload_samples(out);

        return out;
#else
        return {};
#endif
    }

    u32 get_id(blam::tagref_t const& tag)
    {
        return tag.tag_id;
    }
};
