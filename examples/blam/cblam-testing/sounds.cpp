#include "sounds.h"

#include "blam/volta/blam_tag_index.h"
#include "blam/volta/blam_tag_ref.h"
#include "caching_item.h"
#include "coffee/net/curl_context.h"
#include "components.h"
#include "data.h"
#include "data_cache.h"
#include "oaf/api.h"
#include "oaf/wav/wav_decode.h"
#include "peripherals/stl/range.h"
#include "selected_version.h"
#include "sound_cache.h"
#include <algorithm>
#include <magic_enum/magic_enum.hpp>

#include <coffee/net/net_resource.h>
#include <peripherals/stl/string/url_encode.h>

#if defined(FEATURE_ENABLE_OAF)

#if defined(FEATURE_ENABLE_ImGui)
#include <imgui.h>
#endif

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
        oaf::api&        audio,
        SoundCache<Ver>& sound_cache,
        LoadingStatus*   loading,
        GameEventBus&    game_bus)
        : snd(audio)
        , sound_cache(sound_cache)
        , index(sound_cache.index)
        , loading(loading)
    {
        this->priority = 2048;
        game_bus.addEventFunction<ClusterChangedEvent>(
            0, [this](GameEvent&, ClusterChangedEvent* e) {
                on_cluster_changed(e->bsp, e->cluster);
            });
    }

    oaf::api&                  snd;
    SoundCache<Ver>&           sound_cache;
    blam::tag_index_view<Ver>& index;
    LoadingStatus*             loading;

    std::map<u64, sound_unit_t> active_sounds;
    std::map<u64, sound_unit_t> fading_sounds;
    u64                         next_fade_id{0x8000000000000000ULL};

    std::vector<std::shared_ptr<oaf::buffer_t>> buffers;
    std::vector<std::shared_ptr<oaf::source_t>> sources;
    struct voice_synth_t
    {
        char voice[10] = {"KR"};
        char backend[10] = {"melo"};
        char phrase[128] = {"Pee pee poo poo"};
    } voice;

    u32 stat_bg_total{0};   /* cluster changes seen                       */
    u32 stat_bg_with_snd{0};/* ...of which resolved to a non-null sound   */
    u32 stat_started{0};    /* sound_units actually inserted as active   */
    u32 stat_replayed{0};   /* queued events replayed after load         */

    BSPItem const* pending_bsp{nullptr};
    u32            pending_cluster{std::numeric_limits<u32>::max()};
    bool           has_pending_cluster{false};

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

            auto sounds_it = track.sounds.find(meta.active.role);
            auto bufs_it   = track.buffers.find(meta.active.role);
            if(sounds_it == track.sounds.end() ||
               bufs_it == track.buffers.end())
                continue;
            auto const [tag, props, heap] = sounds_it->second;
            auto const& bufs              = bufs_it->second;

            // TODO: Figure out pitch variation
            // Just use natural for now
            if(meta.active.pitch >= bufs.size())
                continue;
            auto const& pitch = bufs.at(meta.active.pitch);
            if(pitch.permutations.empty() ||
               meta.active.permutation >= pitch.permutations.size())
                continue;
            auto const& current_buf =
                pitch.permutations.at(meta.active.permutation);

            if(!meta.source || !current_buf.buffer)
                continue;

            // TODO: If memory is tight, stream audio here instead of preloading
            // cDebug(
            //     "Queueing sound={} perm=#{}",
            //     tag->to_name().to_string(heap),
            //     meta.active.permutation);
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
            stat_replayed += static_cast<u32>(queued_events.size());
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

        if(has_pending_cluster &&
           loading->loaded_sounds == LoadingStatus::loaded)
            apply_pending_cluster();

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

#if defined(FEATURE_ENABLE_ImGui)
        if(ImGui::Begin("Sound"))
        {
            if(ImGui::BeginTabBar("AudioTabs"))
            {
                if(ImGui::BeginTabItem("Tracks"))
                {
                    ImGui::Text(
                        "active=%zu fading=%zu queued=%zu sounds_loaded=%d",
                        active_sounds.size(),
                        fading_sounds.size(),
                        queued_events.size(),
                        (int)(loading->loaded_sounds == LoadingStatus::loaded));
                    ImGui::Text(
                        "lifetime: bg_transitions=%u (with_sound=%u) "
                        "started=%u replayed=%u",
                        stat_bg_total,
                        stat_bg_with_snd,
                        stat_started,
                        stat_replayed);
                    ImGui::Separator();
                    auto sound_row = [&](u64 entity, sound_unit_t const& sound,
                                         ImVec4 color) {
                        auto item_it = sound_cache.find(sound.index);
                        auto name    = index.name_of(sound.source);
                        ImGui::TextColored(color, "[sound/%04lu] %.*s",
                            entity,
                            static_cast<int>(name.size()),
                            name.data());
                        ImGui::Text("    [volume] %f", sound.volume);
                        if(item_it == sound_cache.end())
                        {
                            ImGui::Text("    [not in cache]");
                            return;
                        }
                        SoundItem const& item = item_it->second;
                        u32 track_i{0};
                        for(auto const& track : sound.tracks)
                        {
                            auto role = magic_enum::enum_name(track.active.role);
                            std::string_view name{"[?]"};
                            if(track_i < item.tracks.size())
                            {
                                auto const& track_ = item.tracks[track_i];
                                auto sit = track_.sounds.find(track.active.role);
                                if(sit != track_.sounds.end())
                                {
                                    auto const& [sound_tag, _, __] = sit->second;
                                    if(sound_tag)
                                        name = index.name_of(*sound_tag);
                                }
                            }
                            ++track_i;
                            ImGui::Text("    [track/%02u/%02u/%.*s] %.*s ",
                                track.active.permutation,
                                track.active.pitch,
                                static_cast<int>(role.size()),
                                role.data(),
                                static_cast<int>(name.size()),
                                name.data());
                        }
                        if(!item.detail_sounds.empty())
                        {
                            ImGui::Text("    [detail sounds]");
                            for(auto const& dsound : item.detail_sounds)
                            {
                                for(auto const& [role, snd] : dsound.sounds)
                                {
                                    auto const& [sound_tag, _, __] = snd;
                                    std::string_view name{"[?]"};
                                    if(sound_tag)
                                        name = index.name_of(*sound_tag);
                                    auto role_name = magic_enum::enum_name(role);
                                    ImGui::Text("      [detail/%.*s] %.*s",
                                        static_cast<int>(role_name.size()),
                                        role_name.data(),
                                        static_cast<int>(name.size()),
                                        name.data());
                                }
                            }
                        }
                    };
                    for(auto const& [entity, sound] : active_sounds)
                        sound_row(entity, sound, ImVec4(0, 1, 0, 1));
                    for(auto const& [entity, sound] : fading_sounds)
                        sound_row(entity, sound, ImVec4(0.7, 0.5, 0, 1));
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("Testing"))
                {
                    ImGui::InputText("Voice", voice.voice, sizeof(voice.voice));
                    ImGui::InputText("Backend", voice.backend, sizeof(voice.backend));
                    ImGui::InputText("Phrase", voice.phrase, sizeof(voice.phrase));
                    if(ImGui::Button("Play voice"))
                    {
                        auto src = net::MkUrl(
                            fmt::format("http://10.0.0.17:9006/{}"
                                        "/v1/audio/speech"
                                        "?input={}&voice={}",
                                voice.backend,
                                stl_types::str::url_encode::encode(voice.phrase),
                                voice.voice));
                        net::Resource sound(net::create_curl_context(), src);

                        if(!sound.fetch().has_value() && sound.data().has_value())
                        {
                            buffers.push_back(snd.alloc_buffer());
                            sources.push_back(snd.alloc_source());
                            auto& buf = *buffers.back();
                            auto& src = *sources.back();

                            oaf::decode::wav::decoder wav_dec;
                            wav_dec.decode(
                                sound.data().value(),
                                std::nullopt,
                                std::nullopt,
                                buf);
                            src.queue(buf);
                        } else
                        {
                            cWarning("Got no audio data");
                        }
                    }
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }

        }
        ImGui::End();
#endif
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

    static constexpr u64 background_entity = 0;

    static blam::tagref_t const* resolve_bg_sound(
        BSPItem const* bsp, u32 cluster)
    {
        if(!bsp || cluster >= bsp->clusters.size())
            return nullptr;
        i16 bg_idx = bsp->clusters[cluster].cluster->background_sound;
        if(bg_idx >= 0
           && static_cast<u32>(bg_idx) < bsp->bg_sound_palette.size()
           && bsp->bg_sound_palette[bg_idx])
            return &static_cast<blam::tagref_t const&>(
                bsp->bg_sound_palette[bg_idx]->bg_sound);
        return nullptr;
    }

    void transition_background(blam::tagref_t const* sound)
    {
        auto it = active_sounds.find(background_entity);
        if(it != active_sounds.end())
        {
            if(sound && it->second.source.tag_id == sound->tag_id)
                return;
            // TODO: If the sound has an end part, play that instead
            it->second.fade_rate          = -1.f / 2.f; /* 2s fade out */
            it->second.fading_out         = true;
            fading_sounds[next_fade_id++] = std::move(it->second);
            active_sounds.erase(it);
        }
        if(sound)
        {
            auto unit = make_sound_unit(
                *sound, LoopSoundEvent::usage_t::background_track);
            unit.fade_rate = 1.f / 2.f;
            unit.fading_in = true;
            unit.volume    = 0.f;
            if(unit.index.valid())
            {
                stat_started++;
                active_sounds[background_entity] = std::move(unit);
            }
        }
    }

    void on_cluster_changed(BSPItem const* bsp, u32 cluster)
    {
        pending_bsp         = bsp;
        pending_cluster     = cluster;
        has_pending_cluster = true;
        stat_bg_total++;
        if(loading->loaded_sounds == LoadingStatus::loaded)
            apply_pending_cluster();
    }

    void apply_pending_cluster()
    {
        if(!has_pending_cluster)
            return;
        has_pending_cluster = false;
        auto* sound         = resolve_bg_sound(pending_bsp, pending_cluster);
        if(sound)
            stat_bg_with_snd++;
        transition_background(sound);
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
            transition_background(trans.sound);
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
        &e.subsystem_cast<LoadingStatus>(),
        std::ref(e.subsystem_cast<GameEventBus>()));
    e.register_subsystem_services<SoundSystem<halo_version>>(&sound_sys);
#endif
}
