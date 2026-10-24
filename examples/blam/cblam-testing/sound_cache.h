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
} // namespace oaf
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

    /* The game-neutral fields are what SoundSystem plays from; the Halo 1
     * pointers are kept for the debug UI */
    struct permutation_t
    {
        blam::sound::pitch_permutation_t const* permutation{nullptr};
        std::shared_ptr<oaf::buffer_t>          buffer;
        f32                                     gain{1.f};
        i16                                     next{-1}; /*!< -1 ends the chain */
    };

    struct pitch_range_t
    {
        blam::sound::pitch_range_t const* range{nullptr};
        std::vector<permutation_t>        permutations;
        u32                               count{0}; /*!< Chains that start a playback */
    };

    /*! How a sound plays: its gain and distances (0 = unspatialized) */
    struct props_t
    {
        f32  gain_modifier{1.f};
        f32  min_distance{0.f};
        f32  max_distance{0.f};
        bool music{false};
    };

    struct track_t
    {
        blam::sound::track_t const*                  track{nullptr};
        std::map<role_t, sound_ptr>                  sounds;
        std::map<role_t, std::vector<pitch_range_t>> buffers;
        std::map<role_t, props_t>                    props;
    };

    blam::sound::looping_sound const* looping_sound{nullptr};
    std::vector<track_t>              tracks;
    std::vector<track_t>              detail_sounds;
    std::string                       name;
    bool                              looping{false}; /*!< Loops until stopped */

    /*! A sound played now and then around a loop */
    struct detail_t
    {
        u64   external{0}; /*!< SoundCache::external key */
        Vecf2 period{};    /*!< Seconds between playings */
        f32   gain{1.f};
        Vecf2 yaw{};       /*!< Radians around the loop */
        Vecf2 pitch{};     /*!< Radians above the horizon */
        Vecf2 distance{};
    };

    std::vector<detail_t> details;

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

/* Minimum and maximum distance by sound class, for tags that leave
 * them 0 (the defaults tool and Invader compile in) */
inline Vecf2 default_distances(blam::sound::sound::type_t type)
{
    using sound_t = blam::sound::sound;
    switch(type)
    {
    case sound_t::device_machinery:
    case sound_t::device_force_field:
    case sound_t::ambient_machinery:
    case sound_t::ambient_nature:
    case sound_t::device_door:
    case sound_t::music:
    case sound_t::device_nature:
        return {.9f, 5.f};
    case sound_t::weapon_empty:
    case sound_t::weapon_idle:
    case sound_t::weapon_ready:
    case sound_t::weapon_reload:
    case sound_t::weapon_charge:
    case sound_t::weapon_overheat:
        return {1.f, 9.f};
    case sound_t::scripted_dialog_other:
    case sound_t::scripted_dialog_player:
    case sound_t::game_event:
    case sound_t::unit_dialog:
    case sound_t::scripted_dialog_force_unspatialized:
        return {3.f, 20.f};
    case sound_t::first_person_damage:
    case sound_t::object_impacts:
    case sound_t::ambient_computers:
    case sound_t::particle_impacts:
    case sound_t::device_computers:
    case sound_t::slow_particle_impacts:
        return {.5f, 3.f};
    case sound_t::vehicle_engine:
    case sound_t::projectile_impact:
    case sound_t::vehicle_collision:
        return {1.4f, 8.f};
    case sound_t::weapon_fire:
        return {4.f, 70.f};
    case sound_t::scripted_effect:
        return {2.f, 5.f};
    case sound_t::projectile_detonation:
        return {8.f, 120.f};
    case sound_t::unit_footsteps:
        return {.9f, 10.f};
    default:
        return {};
    }
}

template<typename V>
struct SoundCache
    : DataCache<SoundItem, blam::tag_index<>, blam::tagref_t const&>
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
        m_external.clear();
    }

    inline void load_sounds_from(blam::map_ptr const& sound_ptr)
    {
        index.add_atlas(blam::atlas_type_t::sounds, sound_ptr);
    }

    oaf::api*               api{nullptr};
    blam::tag_index_view<V> index;

    /*! A sound another game's loader decoded itself, under its own key */
    inline generation_idx_t add_external(u64 key, SoundItem item)
    {
        auto id = this->insert(std::move(item));
        if(id.valid())
            m_external[key] = id;
        return id;
    }

    inline generation_idx_t external(u64 key) const
    {
        auto it = m_external.find(key);
        return it == m_external.end() ? generation_idx_t{} : it->second;
    }

    /*! Drops every sound, for a map whose sounds all come from outside */
    inline void clear_external()
    {
        this->evict_all();
        m_external.clear();
    }

    std::map<u64, generation_idx_t> m_external;

#if defined(FEATURE_ENABLE_OAF)
    void upload_singular_samples(
        std::vector<SoundItem::pitch_range_t>& ranges, sound_ptr const& sound_)
    {
        auto [tag, sound, heap] = sound_;
        auto ranges_            = *index.deref(*tag, sound->pitch_ranges_);
        if(ranges_.empty())
            return;
        for(auto const& range : ranges_)
        {
            ranges.emplace_back(
                SoundItem::pitch_range_t{
                    .range = &range,
                    .count = static_cast<u32>(range.actual_permutation_count),
                });
            auto perms_ = index.deref(*tag, range.permutations_);
            if(!perms_.has_value())
                continue;
            auto& out = ranges.back();
            for(auto const& perm : perms_.value())
            {
                out.permutations.emplace_back(
                    SoundItem::permutation_t{
                        .permutation = &perm,
                        .gain        = perm.gain,
                        .next        = perm.next_permutation_idx,
                    });
                auto data_ = index.deref(*tag, perm.sample_data());
                if(!data_.has_value())
                    continue;
                auto  data    = data_.value();
                auto& buffer  = out.permutations.back().buffer;
                buffer        = api->alloc_buffer();
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
                        sound->channels == blam::sound::sound::stereo ? 2 : 1;
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
                    role, std::vector<SoundItem::pitch_range_t>{});
                upload_singular_samples(it.first->second, sound);
                if(auto const* props = std::get<1>(sound))
                {
                    /* Distances of 0 take the sound class's own, as tool fills in */
                    auto const defaults = default_distances(props->type);
                    track.props[role]   = {
                          .gain_modifier = props->gain_modifier,
                          .min_distance  = props->min_distance > 0.f ? props->min_distance : defaults.x,
                          .max_distance  = props->max_distance > 0.f ? props->max_distance : defaults.y,
                          .music         = props->type == blam::sound::sound::music,
                    };
                }
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
        item.looping       = !(lsnd->flags & blam::sound::looping_sound::not_a_loop);

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
                .sounds =
                    {
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

        switch(tag.class_id())
        {
        case blam::tag_class_t::lsnd:
            parse_loop_sound(out, tag);
            break;
        case blam::tag_class_t::snd:
            out.tracks.push_back({.sounds = {
                { SoundItem::main,
                  parse_simple_sound(tag) }
            }});
            break;
        default:
            break;
        }

        // TODO: Make smartness about discarding unplayed buffers
        upload_samples(out);
        out.name = std::string(index.name_of(tag));

        return out;
#else
        return {};
#endif
    }

    blam::tag_index<> get_id(blam::tagref_t const& tag)
    {
        return tag.tag_id;
    }
};
