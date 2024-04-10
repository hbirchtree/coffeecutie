#pragma once

#include "caching_item.h"
#include "data_cache.h"
#include <blam/volta/blam_sound.h>
#include <blam/volta/blam_stl.h>
#include <coffee/components/subsystem.h>
#include <oaf/api.h>

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

    struct track_t
    {
        blam::sound::track_t const* track{nullptr};
        std::map<role_t, sound_ptr> sounds;
    };

    blam::sound::looping_sound const* looping_sound{nullptr};
    sound_ptr                         sound{};

    std::vector<track_t> tracks;

    bool valid() const
    {
        return looping_sound || std::get<1>(sound);
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
    }

    SoundItem predict_impl(blam::tagref_t const& tag)
    {
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
            out.sound = parse_simple_sound(tag);
            break;
        default:
            break;
        }

        return out;
    }

    u32 get_id(blam::tagref_t const& tag)
    {
        return tag.tag_id;
    }
};
