#include "sounds.h"

#include "blam/volta/blam_tag_ref.h"
#include "components.h"
#include "data_cache.h"
#include "peripherals/stl/range.h"
#include "selected_version.h"
#include "sound_cache.h"
#include <algorithm>

#if defined(FEATURE_ENABLE_OAF)

#include <deque>
#include <oaf/api_system.h>
#include <oaf/ogg/ogg_decode.h>
#include <peripherals/stl/enumerate.h>

#if defined(OAF_IMA_DECODER_ENABLED)
#include <oaf/ima_adpcm/decode.h>
#endif

using Coffee::Logging::cDebug;

using SoundManifest = compo::SubsystemManifest<
    type_list_t<SoundEffects>,
    type_list_t<LoadingStatus, SoundPreferences>,
    empty_list_t>;

struct sound_unit_t
{
    struct track_t
    {
        struct
        {
            SoundItem::role_t role{};
            u32               pitch{0};
            u32               permutation{0};
        } active;

        std::deque<std::shared_ptr<oaf::buffer_t>> queued_bufs;
        std::shared_ptr<oaf::source_t>             source;
    };

    blam::tagref_t          source{};
    generation_idx_t        index{};
    std::vector<track_t>    tracks;
    LoopSoundEvent::usage_t usage{LoopSoundEvent::usage_t::general};
    f32                     volume{1.f};
    f32                     fade_rate{0.f}; /* vol/sec, negative = fade out */
    bool                    fading_in{true};
    bool                    fading_out{false};
};

template<typename Ver>
struct SoundSystem
    : compo::RestrictedSubsystem<SoundSystem<Ver>, SoundManifest>
    , comp_app::EventBus<SoundEvent>
{
    using services = type_list_t<comp_app::EventBus<SoundEvent>>;
    using type     = SoundSystem;
    using Proxy    = compo::proxy_of<SoundManifest>;

    SoundSystem(
        oaf::api& audio, SoundCache<Ver>& sound_cache, LoadingStatus* loading)
        : snd(audio)
        , sound_cache(sound_cache)
        , index(sound_cache.index)
        , loading(loading)
    {
    }

    oaf::api&                  snd;
    SoundCache<Ver>&           sound_cache;
    blam::tag_index_view<Ver>& index;
    LoadingStatus*             loading;

    std::map<u64, sound_unit_t> active_sounds;
    std::map<u64, sound_unit_t> fading_sounds;
    u64                         next_fade_id{0x8000000000000000ULL};

    bool              first_frame{true};
    compo::time_point last_t{};

    struct queued_event_t
    {
        SoundEvent event;
        std::variant<
            LoopSoundEvent,
            PlaySoundEvent,
            BackgroundSoundTransitionEvent>
            data;
    };

    std::vector<queued_event_t> queued_events;

    /* Push current sound.volume to every OAF source as gain.
     * Called every frame so fades are smooth regardless of buffer queue state.
     */
    void apply_volume(sound_unit_t& sound, SoundItem const& item)
    {
        for(auto i : stl_types::range<size_t>(item.tracks.size()))
        {
            auto const& track = item.tracks.at(i);
            auto const& meta  = sound.tracks.at(i);

            auto sounds_it = track.sounds.find(meta.active.role);
            if(sounds_it == track.sounds.end())
                continue;
            auto const [tag, props, heap] = sounds_it->second;

            auto bufs_it = track.buffers.find(meta.active.role);
            if(bufs_it == track.buffers.end())
                continue;
            auto const& pitch = bufs_it->second.at(meta.active.pitch);
            if(pitch.permutations.empty() ||
               meta.active.permutation >= pitch.permutations.size())
                continue;

            f32 perm_gain =
                pitch.permutations[meta.active.permutation].permutation->gain;
            meta.source->template set_property<oaf::source_property::gain>(
                props->gain_modifier * perm_gain * sound.volume);
        }
    }

    /* Returns true if the sound played to a non-looping end and should be
     * removed.  Applies effective_volume as the OAF source gain multiplier. */
    bool update_sound_tracks(
        sound_unit_t& sound, SoundItem const& item, f32 effective_volume)
    {
        using role_t  = SoundItem::role_t;
        bool any_done = false;
        for(auto i : stl_types::range<size_t>(item.tracks.size()))
        {
            auto const& track = item.tracks.at(i);
            auto&       meta  = sound.tracks.at(i);

            if(meta.queued_bufs.size() >= 4)
            {
                auto [queued, processed] = meta.source->buffer_queue();
                if(queued >= 4 && processed == 0)
                    continue;
                for(auto _ : stl_types::range<size_t>(processed))
                {
                    meta.source->unqueue(*meta.queued_bufs.front());
                    meta.queued_bufs.pop_front();
                }
            }

            auto const [tag, props, heap] =
                track.sounds.find(meta.active.role)->second;
            auto const& bufs = track.buffers.find(meta.active.role)->second;

            // TODO: Figure out pitch variation
            // Just use natural for now
            auto const& pitch = bufs.at(meta.active.pitch);
            if(pitch.permutations.empty())
                continue;
            auto const& current_buf =
                pitch.permutations.at(meta.active.permutation);

            // TODO: If memory is tight, stream audio here instead of preloading
            cDebug(
                "Queueing sound={} perm=#{}",
                tag->to_name().to_string(heap),
                meta.active.permutation);
            meta.source->queue(*current_buf.buffer);
            meta.source->template set_property<oaf::source_property::gain>(
                props->gain_modifier * current_buf.permutation->gain *
                effective_volume);
            meta.queued_bufs.push_back(current_buf.buffer);

            bool looping = item.looping_sound;
            bool eos_permutation =
                current_buf.permutation->next_permutation_idx == -1;

            if(looping && eos_permutation)
            {
                switch(meta.active.role)
                {
                case role_t::start:
                    meta.active.role        = role_t::loop;
                    meta.active.permutation = 0;
                    break;
                case role_t::loop:
                    // TODO: Add 50/50 chance of alt_loop
                    meta.active.permutation = 0;
                    break;
                case role_t::alt_loop:
                    meta.active.role        = role_t::loop;
                    meta.active.permutation = 0;
                    break;
                default:
                    // TODO: Figure out when to play end
                    break;
                }
            } else if(eos_permutation)
            {
                any_done = true;
            } else
            {
                meta.active.permutation =
                    current_buf.permutation->next_permutation_idx;
            }
        }
        return any_done;
    }

    void start_restricted(Proxy& p, compo::time_point const& t)
    {
        SoundPreferences* sound_pref{};
        p.subsystem(sound_pref);
        snd.listener().template set_property<oaf::listener_property::gain>(
            sound_pref->master_volume);

        f32 dt = 0.f;
        if(!first_frame)
            dt = std::chrono::duration<f32>(t - last_t).count();
        first_frame = false;
        last_t      = t;

        if(!queued_events.empty() &&
           loading->loaded_sounds == LoadingStatus::loaded)
        {
            for(queued_event_t& event : queued_events)
            {
                switch(event.event.type)
                {
                case SoundEvent::loop_sound:
                    process(event.event, &std::get<LoopSoundEvent>(event.data));
                    break;
                case SoundEvent::play_sound:
                    process(event.event, &std::get<PlaySoundEvent>(event.data));
                    break;
                case SoundEvent::background_sound_transition:
                    process(
                        event.event,
                        &std::get<BackgroundSoundTransitionEvent>(event.data));
                    break;
                default:
                    break;
                }
            }
            queued_events.clear();
        }

        std::vector<u64> finished;
        for(auto& [id, sound] : active_sounds)
        {
            SoundItem const& item = (*sound_cache.find(sound.index)).second;
            if(sound.fading_in)
            {
                sound.volume =
                    std::min(1.f, sound.volume + sound.fade_rate * dt);
                apply_volume(sound, item);
                if(sound.volume >= 1.f)
                    sound.fading_in = false;
            }
            if(update_sound_tracks(sound, item, sound.volume))
                finished.push_back(id);
        }
        for(auto id : finished)
            active_sounds.erase(id);

        std::vector<u64> fading_finished;
        for(auto& [id, sound] : fading_sounds)
        {
            SoundItem const& item = (*sound_cache.find(sound.index)).second;
            sound.volume = std::max(0.f, sound.volume + sound.fade_rate * dt);
            apply_volume(sound, item);
            update_sound_tracks(sound, item, sound.volume);
            if(sound.volume <= 0.f)
                fading_finished.push_back(id);
        }
        for(auto id : fading_finished)
            fading_sounds.erase(id);
    }

    void end_restricted(Proxy&, compo::time_point const&)
    {
    }

    sound_unit_t make_sound_unit(
        blam::tagref_t const& tagref, LoopSoundEvent::usage_t usage)
    {
        auto sound = sound_cache.predict(tagref);
        if(!sound.valid())
            return {};
        SoundItem const& item  = (*sound_cache.find(sound)).second;
        auto select_first_role = [](SoundItem::track_t const& track) {
            if(track.sounds.find(SoundItem::role_t::start) !=
               track.sounds.end())
                return SoundItem::role_t::start;
            return SoundItem::role_t::loop;
        };
        std::vector<sound_unit_t::track_t> tracks;
        for(auto const& track : item.tracks)
        {
            tracks.emplace_back(
                sound_unit_t::track_t{
                    .active = {.role = select_first_role(track)},
                    .source = snd.alloc_source(),
                });
        }
        return sound_unit_t{
            .source = tagref,
            .index  = sound,
            .tracks = std::move(tracks),
            .usage  = usage,
        };
    }

    virtual void process(SoundEvent& ev, libc_types::c_ptr data) final
    {
        /* Immediate regardless of loading state. */
        if(ev.type == SoundEvent::clear_all)
        {
            active_sounds.clear();
            fading_sounds.clear();
            queued_events.clear();
            return;
        }
        if(ev.type == SoundEvent::stop_sound)
        {
            active_sounds.erase(ev.entity_id);
            return;
        }

        /* Queue events until sound assets are ready. */
        if(loading->loaded_sounds != LoadingStatus::loaded)
        {
            queued_event_t event{.event = ev};
            switch(ev.type)
            {
            case SoundEvent::loop_sound:
                event.data = *reinterpret_cast<LoopSoundEvent const*>(data);
                break;
            case SoundEvent::play_sound:
                event.data = *reinterpret_cast<PlaySoundEvent const*>(data);
                break;
            case SoundEvent::background_sound_transition:
                event.data =
                    *reinterpret_cast<BackgroundSoundTransitionEvent const*>(
                        data);
                break;
            default:
                return;
            }
            queued_events.push_back(event);
            return;
        }

        if(ev.type == SoundEvent::background_sound_transition)
        {
            auto const& trans =
                *reinterpret_cast<BackgroundSoundTransitionEvent const*>(data);

            auto it = active_sounds.find(ev.entity_id);
            if(it != active_sounds.end())
            {
                if(trans.sound)
                    if(it->second.source.tag_id == trans.sound->tag_id)
                        return;

                it->second.fade_rate  = -1.f / 2.f; /* 2-second fade out */
                it->second.fading_out = true;
                fading_sounds[next_fade_id++] = std::move(it->second);
                active_sounds.erase(it);
            }

            if(trans.sound)
            {
                auto unit = make_sound_unit(
                    *trans.sound, LoopSoundEvent::usage_t::background_track);
                unit.fade_rate = 1.f / 2.f;
                unit.fading_in = true;
                unit.volume    = 0.f;
                if(unit.index.valid())
                    active_sounds[ev.entity_id] = std::move(unit);
            }
            return;
        }

        if(ev.type == SoundEvent::loop_sound)
        {
            auto const& loop = reinterpret_cast<LoopSoundEvent const*>(data);
            auto        unit = make_sound_unit(*loop->sound, loop->usage);
            if(loop->usage == LoopSoundEvent::usage_t::background_track)
            {
                unit.fade_rate = 1.f / 2.f;
                unit.fading_in = true;
                unit.volume    = 0.f;
            }
            if(unit.index.valid())
                active_sounds[ev.entity_id] = std::move(unit);
        }
        if(ev.type == SoundEvent::play_sound)
        {
            auto const& play  = reinterpret_cast<PlaySoundEvent const*>(data);
            auto        sound = sound_cache.predict(*play->sound);
        }
    }
};

#endif

void alloc_sound_system(compo::EntityContainer& e)
{
#if defined(FEATURE_ENABLE_OAF)
    ProfContext _;
    e.register_subsystem_inplace<SoundPreferences>();
    auto& sound_sys = e.register_subsystem_inplace<SoundSystem<halo_version>>(
        std::ref(e.subsystem_cast<oaf::system>()),
        std::ref(e.subsystem_cast<SoundCache<halo_version>>()),
        &e.subsystem_cast<LoadingStatus>());
    e.register_subsystem_services<SoundSystem<halo_version>>(&sound_sys);
#endif
}
