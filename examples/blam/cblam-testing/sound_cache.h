#pragma once

#include "caching_item.h"
#include "data_cache.h"
#include <blam/volta/blam_sound.h>
#include <blam/volta/blam_stl.h>
#include <coffee/components/subsystem.h>
#include <oaf/api.h>

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

    struct track_t
    {
        blam::sound::track_t const*                 track{nullptr};
        std::map<role_t, blam::sound::sound const*> sounds;
    };

    blam::sound::looping_sound const* looping_sound{nullptr};
    blam::sound::sound const*         sound{nullptr};

    std::vector<track_t> tracks;

    bool valid() const
    {
        return looping_sound || sound;
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
        magic = map.magic;
        if(map.map->version == blam::version_t::xbox)
            sound_magic = map.magic;
        evict_all();
    }

    inline void load_sounds_from(blam::magic_data_t const& sound_magic)
    {
        this->sound_magic              = sound_magic;
        this->sound_magic.magic_offset = 0;
    }

    oaf::api*               api{nullptr};
    blam::tag_index_view<V> index;
    blam::magic_data_t      magic;
    blam::magic_data_t      sound_magic;

    blam::sound::sound const* parse_simple_sound(blam::tagref_t const& tag)
    {
        return index.template data<blam::sound::sound>(tag).value_or(nullptr);
    }

    blam::sound::sound const* parse_simple_sound(
        blam::tagref_typed_t<blam::tag_class_t::snd> const& tag)
    {
        return parse_simple_sound(tag.to_plain());
    }

    void parse_loop_sound(SoundItem& item, blam::tag_t const& tag)
    {
        auto const* lsnd = tag.data<blam::sound::looping_sound>(magic).value();
        item.looping_sound = lsnd;

        auto tracks = lsnd->tracks.data(magic).value();
        for(blam::sound::track_t const& track : tracks)
        {
            item.tracks.push_back({
                .track = &track,
            });
            auto& sounds             = item.tracks.back().sounds;
            sounds[SoundItem::start] = parse_simple_sound(track.start);
            sounds[SoundItem::loop]  = parse_simple_sound(track.loop);
            sounds[SoundItem::end]   = parse_simple_sound(track.end);
            std::erase_if(
                sounds, [](auto const& p) { return p.second == nullptr; });
        }
    }

    SoundItem predict_impl(blam::tagref_t const& tag)
    {
        if(std::is_same_v<V, blam::custom_version_t>)
            return {};

        if(!tag.matches(blam::tag_class_t::snd) &&
           !tag.matches(blam::tag_class_t::lsnd))
            return {};
        SoundItem out;

        auto tag_it = index.find(tag);

        if(tag_it == index.end())
            return {};

        cDebug("Sound:");
        switch(tag.tag_class)
        {
        case blam::tag_class_t::lsnd:
            parse_loop_sound(out, *tag_it);
            break;
        case blam::tag_class_t::snd:
            out.sound = parse_simple_sound(tag);
            break;
        default:
            break;
        }

        return out;
    }

    void process_sounds()
    {
        for(auto const& [key, sound] : m_cache)
        {
            if(!sound.looping_sound)
                continue;
            for(SoundItem::track_t const& track : sound.tracks)
            {
                if(!track.sounds.contains(SoundItem::loop))
                    continue;
                cDebug(
                    " - Track: {} sounds, loop=codec={};samplerate={}",
                    track.sounds.size(),
                    magic_enum::enum_name(
                        track.sounds.at(SoundItem::loop)->codec),
                    magic_enum::enum_name(
                        track.sounds.at(SoundItem::loop)->sample_rate));
                auto sound   = track.sounds.at(SoundItem::loop);
                auto pitches = track.sounds.at(SoundItem::loop)
                                   ->pitch_ranges.data(magic)
                                   .value();
                for(auto const& pitch : pitches)
                {
                    auto perms = pitch.permutations.data(magic).value();
                    for(auto const& perm : perms)
                    {
                        auto data =
                            perm.sample_data().data(sound_magic).value();
                        cDebug(
                            "   - Perm: gain={}, ptr={}",
                            perm.gain,
                            static_cast<const void*>(data.data()));
                    }
                }
            }
        }
    }

    u32 get_id(blam::tagref_t const& tag)
    {
        return tag.tag_id;
    }
};
