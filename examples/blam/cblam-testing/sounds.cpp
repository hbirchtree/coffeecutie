#include "sounds.h"

#include "blam/volta/blam_tag_index.h"
#include "blam/volta/blam_tag_ref.h"
#include "caching.h"
#include "caching_item.h"
#include "coffee/net/curl_context.h"
#include "components.h"
#include "data.h"
#include "data_cache.h"
#include "peripherals/concepts/sound_api.h"
#include "peripherals/stl/range.h"
#include "peripherals/stl/type_list.h"
#include "proxy.h"
#include "selected_version.h"
#include "sound_cache.h"
#include "subsystem.h"
#include "types.h"
#include <algorithm>
#include <random>
#include <glm/matrix.hpp>
#include <magic_enum/magic_enum.hpp>

#include <coffee/net/net_resource.h>
#include <peripherals/stl/string/url_encode.h>

#if defined(FEATURE_ENABLE_OAF)

#if defined(FEATURE_ENABLE_ImGui)
#include <imgui.h>
#endif

#include <deque>
#include <mutex>
#include <oaf/api_system.h>
#include <oaf/ogg/ogg_decode.h>
#include <oaf/wav/wav_decode.h>
#include <peripherals/stl/enumerate.h>
#include <queue>
#include <ranges>
#include <unordered_map>

#if defined(OAF_IMA_DECODER_ENABLED)
#include <oaf/ima_adpcm/decode.h>
#endif

using Coffee::Logging::cDebug;

template<typename Ver>
using SoundManifest = compo::SubsystemManifest<
    type_list_t<const PlayerCamera, const PlayerInfo, const SoundEffects>,
    type_list_t<
        const LoadingStatus,
        const SoundPreferences,
        SoundCache<Ver>,
        const BSPCache<Ver>>,
    empty_list_t>;

struct sound_unit_t
{
    struct track_t
    {
        struct
        {
            SoundItem::role_t role{};
            u32               pitch{0};
            u32               permutation_group{0};
            u32               permutation{0};
        } active;

        std::deque<std::shared_ptr<oaf::buffer_t>> queued_bufs{};
        std::shared_ptr<oaf::source_t>             source;
        std::shared_ptr<oaf::filter_t>             filter; /* occlusion */
        std::shared_ptr<oaf::filter_t>             send_filter;
        f32 base_gain{1.f}; /* before occlusion, without EFX */
        f32 max_distance{0.f}; /* snd! max distance, 0 = unlimited */
        f32 range_applied{1.f};
    };

    blam::tagref_t          source{};
    generation_idx_t        index{};
    std::vector<track_t>    tracks;
    LoopSoundEvent::usage_t usage{LoopSoundEvent::usage_t::general};
    f32                     volume{1.f};
    f32                     gain_scale{1.f}; /* from UpdateSoundEvent */
    f32                     fade_rate{0.f}; /* vol/sec, negative = fade out */
    bool                    fading_in{true};
    bool                    fading_out{false};
    bool                    queued_all{false};
    bool                    finished{false};

    /* World position, empty for listener-relative sounds */
    std::optional<Vecf3> position{};
    f32                  occlusion{0.f}; /* smoothed, 0 = clear */
    f32                  occlusion_applied{-1.f};
    f32                  occlusion_target{0.f}; /* fraction of rays blocked */
    bool                 has_sends{false};

    /* Walk from each portal to the sound, used to move the apparent
     * position towards the openings when the direct path is blocked */
    struct portal_cost_t
    {
        f32   cost{0.f}; /* from the centroid */
        f32   rest{0.f}; /* from next */
        Vecf3 next{};    /* where the walk continues towards the sound */
        Vecf3 normal{};
        blam::bsp::cluster_portal const* next_portal{}; /* null at the sound */
    };

    std::unordered_map<blam::bsp::cluster_portal const*, portal_cost_t>
                       portal_costs{};
    BSPItem const*     route_bsp{nullptr};
    std::optional<u32> cluster{}; /* of the sound, for route_bsp */
    /* Nearest open point, sounds are often placed inside a wall or device */
    std::optional<Vecf3> open_position{};
    Vecf3                route_origin{}; /* sound position the costs are from */
    /* Smoothed turn and extra distance from the real position */
    Quatf route_turn{1.f, 0.f, 0.f, 0.f};
    f32   route_extra{0.f};
    f32   route_distance{0.f}; /* 0 = direct */
    Vecf3 position_applied{};

    // Not used for audio tracks, more for standalone sounds
    std::optional<compo::time_point> time{};
};

template<typename Ver>
struct SoundSystem
    : compo::RestrictedSubsystem<SoundSystem<Ver>, SoundManifest<Ver>>
    , comp_app::EventBus<SoundEvent>
{
    using services = type_list_t<comp_app::EventBus<SoundEvent>>;
    using type     = SoundSystem;
    using Proxy    = compo::proxy_of<SoundManifest<Ver>>;

    SoundSystem(
        oaf::api&            audio,
        SoundCache<Ver>&     sound_cache,
        LoadingStatus const* loading,
        GameEventBus&        game_bus)
        : snd(audio)
        , sound_cache(sound_cache)
        , index(sound_cache.index)
        , loading(loading)
    {
        this->priority = 2048;
        cluster_events = game_bus.addQueuedEventFunction<ClusterChangedEvent>(
            0, [this](GameEvent&, ClusterChangedEvent* e) {
                on_cluster_changed(e->bsp, e->cluster);
            });
    }

    bool parallel_safe() const override
    {
        return true;
    }

    oaf::api&                  snd;
    SoundCache<Ver>&           sound_cache;
    blam::tag_index_view<Ver>& index;
    LoadingStatus const*       loading;
    stl_types::math::rng       random;

    std::shared_ptr<GameEventBus::queue_type<ClusterChangedEvent>>
        cluster_events;

    /* Cluster reverb, two slots so environment changes crossfade.
     * Declared before the sounds, a slot can't be deleted while in use */
    struct environment_t
    {
        std::shared_ptr<oaf::effect_slot_t> slots[2];
        f32                   gain[2]{0.f, 0.f};
        f32                   target{0.f}; /* of the active slot */
        u32                   active{0};
        blam::tagref_t const* ref{nullptr};
        bool                  initialized{false};
        bool                  enabled{true};
    } environment;

    Vecf3 listener_pos{};
    bool  has_listener{false};
    bool  occlusion_enabled{true};
    bool  portal_routing{true};
    bool  occlusion_gain_only{false}; /* the no-EFX path, for tuning */
    bool  occlusion_efx_applied{false};

    /* Occlusion response, tunable from the Sound panel. Cuts are what is
     * lost when every ray is blocked; curve > 1 keeps partial blocking
     * (around a corner) mild while fully enclosed sounds drop hard */
    struct occlusion_tuning_t
    {
        f32 dry_gain{0.65f};
        f32 dry_hf{0.9f};
        f32 wet_gain{0.7f}; /* reverb send, only near full occlusion */
        f32 gain_only{0.75f};
        f32 curve{2.f};
        /* Neither is in the tags (snde room rolloff applies on top), so
         * both default to off. Air absorption is per meter, ~3m/unit */
        f32 room_rolloff{0.f};
        f32 air_absorption{0.f};
        /* Seconds, for occlusion and the routed position */
        f32 smoothing{0.3f};
        /* Units, how much longer a portal's way may be and still pull */
        f32 portal_spread{2.f};

        bool operator==(occlusion_tuning_t const&) const = default;
    } occlusion_tuning, occlusion_tuning_applied;

    std::map<u64, sound_unit_t> active_sounds;
    std::map<u64, sound_unit_t> fading_sounds;
    u64                         next_fade_id{0x8000000000000000ULL};

    std::vector<sound_unit_t> singleshot_sounds;

    std::vector<std::shared_ptr<oaf::buffer_t>> buffers;
    std::vector<std::shared_ptr<oaf::source_t>> sources;

    struct voice_synth_t
    {
        char voice[10]   = {"KR"};
        char backend[10] = {"melo"};
        char phrase[128] = {"Pee pee poo poo"};
    } voice;

    u32 stat_bg_total{0};    /* cluster changes seen                       */
    u32 stat_bg_with_snd{0}; /* ...of which resolved to a non-null sound   */
    u32 stat_started{0};     /* sound_units actually inserted as active   */
    u32 stat_replayed{0};    /* queued events replayed after load         */

    BSPItem const* pending_bsp{nullptr};
    u32            pending_cluster{std::numeric_limits<u32>::max()};
    bool           has_pending_cluster{false};

    bool              first_frame{true};
    compo::time_point last_t{};

    struct queued_event_t
    {
        SoundEvent event;
        std::variant<
            std::monostate,
            LoopSoundEvent,
            PlaySoundEvent,
            BackgroundSoundTransitionEvent,
            UpdateSoundEvent>
            data{};
    };

    /*! Raised before the sound assets were there, replayed once they are */
    std::vector<queued_event_t> queued_events;

    /*! Raised on another thread, handed over at the next frame hook */
    std::vector<queued_event_t> inbox;
    std::mutex                  inbox_lock;

    /* Push current sound.volume to every OAF source as gain.
     * Called every frame so fades are smooth regardless of buffer queue state.
     */
    void apply_volume(sound_unit_t& sound, SoundItem const& item)
    {
        for(auto i : stl_types::range<size_t>(item.tracks.size()))
        {
            auto const& track = item.tracks.at(i);
            auto&       meta  = sound.tracks.at(i);

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
            set_gain(
                sound,
                meta,
                props->gain_modifier * perm_gain * sound.volume *
                    sound.gain_scale);
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
            set_gain(
                sound,
                meta,
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
                    // Check first which permutation group it's at
                    // In title theme this has three groups, so switch between them
                    meta.active.permutation_group ++;
                    if(meta.active.permutation_group < pitch.range->actual_permutation_count)
                    {
                        // For now be dumb and simply advance
                        meta.active.permutation = meta.active.permutation_group;
                    } else
                    {
                        meta.active.permutation       = 0;
                        meta.active.permutation_group = 0;
                    }
                    break;
                case role_t::alt_loop:
                    meta.active.role        = role_t::loop;
                    meta.active.permutation = 0;
                    break;
                case role_t::end:
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

    void update_singleshot_sound(sound_unit_t& sound)
    {
        if(sound.queued_all)
        {
            bool playing = false;
            for(auto& meta : sound.tracks)
            {
                if(!meta.source)
                    continue;
                auto [queued, processed] = meta.source->buffer_queue();
                playing = playing || processed < queued;
            }
            sound.finished = !playing;
            return;
        }
        sound.queued_all = true;

        SoundItem const& item = (*sound_cache.find(sound.index)).second;
        for(auto i : stl_types::range<size_t>(item.tracks.size()))
        {
            auto const& track = item.tracks.at(i);
            auto&       meta  = sound.tracks.at(i);

            auto sounds_it = track.sounds.find(meta.active.role);
            auto bufs_it   = track.buffers.find(meta.active.role);
            if(!meta.source || sounds_it == track.sounds.end() ||
               bufs_it == track.buffers.end())
                continue;
            auto const [tag, props, heap] = sounds_it->second;
            auto const& bufs              = bufs_it->second;

            if(meta.active.pitch >= bufs.size())
                continue;
            auto const& pitch = bufs.at(meta.active.pitch);
            if(meta.active.permutation >= pitch.permutations.size())
                continue;

            set_gain(
                sound,
                meta,
                props->gain_modifier *
                    pitch.permutations.at(meta.active.permutation)
                        .permutation->gain);

            /* The whole chain goes in at once, bounded in case it is cyclic */
            u32 perm = meta.active.permutation;
            for(auto _ : stl_types::range<size_t>(pitch.permutations.size()))
            {
                auto const& buf = pitch.permutations.at(perm);
                if(!buf.buffer)
                    break;
                meta.source->queue(*buf.buffer);
                meta.queued_bufs.push_back(buf.buffer);

                i16 next = buf.permutation->next_permutation_idx;
                if(next < 0 ||
                   static_cast<u32>(next) >= pitch.permutations.size())
                    break;
                perm = static_cast<u32>(next);
            }
        }
    }

    /*! Copy an event's payload so it survives the trip to this thread */
    static queued_event_t capture(SoundEvent const& ev, libc_types::c_ptr data)
    {
        queued_event_t out{.event = ev};

        if(!data)
            return out;

        switch(ev.type)
        {
        case SoundEvent::loop_sound:
            out.data = *reinterpret_cast<LoopSoundEvent const*>(data);
            break;
        case SoundEvent::play_sound:
            out.data = *reinterpret_cast<PlaySoundEvent const*>(data);
            break;
        case SoundEvent::background_sound_transition:
            out.data =
                *reinterpret_cast<BackgroundSoundTransitionEvent const*>(data);
            break;
        case SoundEvent::update_sound:
            out.data = *reinterpret_cast<UpdateSoundEvent const*>(data);
            break;
        default:
            break;
        }

        return out;
    }

    void deliver(queued_event_t& queued)
    {
        std::visit(
            [this, &queued](auto& payload) {
                using payload_t = std::decay_t<decltype(payload)>;
                if constexpr(std::is_same_v<payload_t, std::monostate>)
                    process(queued.event, nullptr);
                else
                    process(queued.event, &payload);
            },
            queued.data);
    }

    virtual void inject(SoundEvent& ev, libc_types::c_ptr data) final
    {
        std::lock_guard _(inbox_lock);
        inbox.push_back(capture(ev, data));
    }

    void drain_inbox()
    {
        std::vector<queued_event_t> incoming;
        {
            std::lock_guard _(inbox_lock);
            incoming.swap(inbox);
        }

        for(auto& queued : incoming)
            deliver(queued);
    }

    void start_restricted(Proxy& p, compo::time_point const& t)
    {
        init_environment();
        cluster_events->poll();
        drain_inbox();

        SoundPreferences const* sound_pref{};
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

            /* Replaying can re-queue, so hand over the list first */
            std::vector<queued_event_t> replay;
            replay.swap(queued_events);

            for(queued_event_t& event : replay)
                deliver(event);
        }

        if(has_pending_cluster &&
           loading->loaded_sounds == LoadingStatus::loaded)
            apply_pending_cluster();
        update_environment(dt);
        update_occlusion(p, dt);

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
            if(update_sound_tracks(
                   sound, item, sound.volume * sound.gain_scale))
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

        for(auto& sound : singleshot_sounds)
        {
            update_singleshot_sound(sound);
        }
        auto num_cleared = std::erase_if(singleshot_sounds, [](sound_unit_t const& unit) {
            return unit.finished;
        });
        if(num_cleared > 0)
            cDebug("Cleaned up {} singleshot sounds", num_cleared);

        for(auto const player : p.template select<PlayerInfo, PlayerCamera>())
        {
            auto const [info, cam] = player.components();
            if(info.seat_idx != 0)
                continue;
            auto const& cached   = cam.camera_.cached;
            auto&       listener = snd.listener();
            listener_pos         = cam.camera.position;
            has_listener         = true;
            listener.template set_property<oaf::listener_property::position>(
                cam.camera.position);
            listener.template set_property<oaf::listener_property::orientation>(
                glm::transpose(
                    glm::mat3(cached.right, cached.up, -cached.forward)));
        }
    }

    sound_unit_t make_sound_unit(
        blam::tagref_t const& tagref, LoopSoundEvent::usage_t usage)
    {
        auto sound = sound_cache.predict(tagref);
        if(!sound.valid())
        {
            cWarning("Tried to load sound, failed: {}", index.name_of(tagref));
            return {};
        }
        SoundItem const& item  = (*sound_cache.find(sound)).second;
        auto select_first_role = [](SoundItem::track_t const& track) {
            if(track.sounds.contains(SoundItem::role_t::start))
                return SoundItem::role_t::start;
            /* Plain snd! tags only have a main track */
            if(track.sounds.contains(SoundItem::role_t::main))
                return SoundItem::role_t::main;
            return SoundItem::role_t::loop;
        };
        std::vector<sound_unit_t::track_t> tracks;
        for(auto const& track : item.tracks)
        {
            auto& meta = tracks.emplace_back(
                sound_unit_t::track_t{
                    .active = {.role = select_first_role(track)},
                    .source = snd.alloc_source(),
                });
            /* A one-shot plays one of its permutations, picked at random;
             * each chains through its own pieces to the end */
            if(!item.looping_sound)
                if(auto bufs = track.buffers.find(meta.active.role);
                   bufs != track.buffers.end() && !bufs->second.empty() &&
                   bufs->second.front().range)
                {
                    u32 const count = std::min<u32>(
                        bufs->second.front().range->actual_permutation_count,
                        bufs->second.front().permutations.size());
                    if(count > 1)
                    {
                        u32 const pick =
                            std::uniform_int_distribution<u32>(0, count - 1)(
                                m_permutation_rng);
                        meta.active.permutation_group = pick;
                        meta.active.permutation       = pick;
                    }
                }
            auto sounds_it = track.sounds.find(meta.active.role);
            if(sounds_it == track.sounds.end())
                continue;
            auto const props = std::get<1>(sounds_it->second);
            /* Music is mixed already, panning and HRTF only colour it */
            if(props && props->type == blam::sound::sound::music)
                meta.source->set_direct_channels(true);
            if(!props || props->min_distance <= 0.f)
                continue;
            /* DirectSound 3D: full volume inside min distance, inverse
             * rolloff past it, nothing past max distance */
            meta.source->template set_property<
                oaf::source_property::reference_distance>(props->min_distance);
            meta.source->template set_property<
                oaf::source_property::rolloff_factor>(1.f);
            meta.max_distance = props->max_distance;
        }
        cDebug("Created sound unit tracks={}", tracks.size());
        return sound_unit_t{
            .source = tagref,
            .index  = sound,
            .tracks = std::move(tracks),
            .usage  = usage,
        };
    }

    std::minstd_rand m_permutation_rng{std::random_device{}()};

    static constexpr u64 background_entity = 0;
    /* Menu music has no cluster, so cluster changes must not replace it */
    static constexpr u64 title_entity = 0x7fffffffffffffffULL;

    static blam::tagref_t const* resolve_bg_sound(
        BSPItem const* bsp, u32 cluster)
    {
        if(!bsp || cluster >= bsp->clusters.size())
            return nullptr;
        i16 bg_idx = bsp->clusters[cluster].cluster->background_sound;
        if(bg_idx >= 0 &&
           static_cast<u32>(bg_idx) < bsp->bg_sound_palette.size() &&
           bsp->bg_sound_palette[bg_idx])
            return &static_cast<blam::tagref_t const&>(
                bsp->bg_sound_palette[bg_idx]->bg_sound);
        return nullptr;
    }

    void transition_background(
        blam::tagref_t const* sound,
        blam::tag_t const* sound_tag = nullptr)
    {
        blam::tagref_t tmp{};
        u64            slot = background_entity;
        if(sound_tag)
        {
            // Synthesize a fake tagref_t
            // Hopefully only needed for the title track?
            tmp   = sound_tag->as_ref();
            sound = &tmp;
            slot  = title_entity;
        }
        if(sound)
            cDebug("Background music transition: {}", index.name_of(*sound));
        else
            cDebug("Clearing background music");
        auto it = active_sounds.find(slot);
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
                active_sounds[slot] = std::move(unit);
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
        transition_environment(
            resolve_sound_env(pending_bsp, pending_cluster));
    }

    static blam::tagref_t const* resolve_sound_env(
        BSPItem const* bsp, u32 cluster)
    {
        if(!bsp || cluster >= bsp->clusters.size())
            return nullptr;
        i16 env_idx = bsp->clusters[cluster].cluster->sound_env;
        if(env_idx < 0 ||
           static_cast<u32>(env_idx) >= bsp->sound_env_palette.size() ||
           !bsp->sound_env_palette[env_idx])
            return nullptr;
        return &static_cast<blam::tagref_t const&>(
            bsp->sound_env_palette[env_idx]->environment);
    }

    void init_environment()
    {
        auto& env = environment;
        if(env.initialized)
            return;
        env.initialized = true;
        for(auto& slot : env.slots)
        {
            slot = snd.alloc_effect_slot();
            if(!slot)
                return;
            slot->set_gain(0.f);
        }
    }

    void transition_environment(blam::tagref_t const* ref)
    {
        auto& env = environment;
        if(!env.slots[0] || !env.slots[1])
            return;
        if(ref == env.ref || (ref && env.ref && ref->tag_id == env.ref->tag_id))
            return;
        env.ref = ref;

        blam::sound::environment const* snde{nullptr};
        if(ref)
            if(auto data = index.template data<blam::sound::environment>(*ref))
                snde = data.value();

        /* The old environment fades out in the other slot */
        env.active ^= 1;
        env.target = snde ? 1.f : 0.f;
        if(snde)
            env.slots[env.active]->set_reverb(oaf::reverb_t{
                .density           = snde->density,
                .diffusion         = snde->diffusion,
                .gain              = snde->room_intensity,
                .gain_hf           = snde->room_intensity_hf,
                .decay_time        = snde->decay_time,
                .decay_hf_ratio    = snde->decay_hf_ratio,
                .reflections_gain  = snde->reflections_intensity,
                .reflections_delay = snde->reflections_delay,
                .late_reverb_gain  = snde->reverb_intensity,
                .late_reverb_delay = snde->reverb_delay,
                .room_rolloff      = snde->room_rolloff,
                .hf_reference      = snde->hf_reference,
            });
        if(ref)
            cDebug("Sound environment: {}", index.name_of(*ref));
        else
            cDebug("Sound environment: none");
    }

    void update_environment(f32 dt)
    {
        auto& env = environment;
        if(!env.slots[0] || !env.slots[1])
            return;
        constexpr f32 fade_rate = 1.f; /* per second */
        for(u32 i : {0u, 1u})
        {
            f32 target =
                (i == env.active && env.enabled) ? env.target : 0.f;
            f32 gain = env.gain[i];
            gain     = gain < target ? std::min(target, gain + fade_rate * dt)
                                     : std::max(target, gain - fade_rate * dt);
            if(gain == env.gain[i])
                continue;
            env.gain[i] = gain;
            env.slots[i]->set_gain(gain);
        }
    }

    bool occlusion_uses_efx() const
    {
        return snd.features().efx && !occlusion_gain_only;
    }

    /* Every gain write goes through here so the gain-only occlusion path
     * is not overwritten by fades and queueing */
    void set_gain(
        sound_unit_t const& unit, sound_unit_t::track_t& meta, f32 gain)
    {
        meta.base_gain     = gain;
        meta.range_applied = range_gain(unit, meta);
        f32 occlusion      = occlusion_uses_efx() ? 0.f : unit.occlusion;
        meta.source->template set_property<oaf::source_property::gain>(
            gain * meta.range_applied *
            (1.f - occlusion_tuning.gain_only *
                       std::pow(occlusion, occlusion_tuning.curve)));
    }

    /* Fades out over the last part of max distance instead of cutting */
    f32 range_gain(
        sound_unit_t const& unit, sound_unit_t::track_t const& meta) const
    {
        if(!unit.position || !has_listener || meta.max_distance <= 0.f)
            return 1.f;
        constexpr f32 fade_fraction = 0.2f;
        f32 const distance = glm::distance(unit.position_applied, listener_pos);
        return 1.f - glm::smoothstep(
                         meta.max_distance * (1.f - fade_fraction),
                         meta.max_distance,
                         distance);
    }

    /* The range fade follows the listener, not only gain writes */
    void apply_range(sound_unit_t& unit)
    {
        for(auto& track : unit.tracks)
        {
            f32 const range = range_gain(unit, track);
            if(std::abs(range - track.range_applied) > 0.005f)
                set_gain(unit, track, track.base_gain);
        }
    }

    static BSPItem const* active_bsp(BSPCache<Ver> const& cache)
    {
        for(auto const& [id, candidate] : cache.m_cache)
            if(candidate.valid() &&
               candidate.section_idx == cache.active_section)
                return &candidate;
        return nullptr;
    }

    /* Fraction of a ray fan that is blocked: the direct line, plus offsets
     * around both ends so edges and doorways give partial values */
    static f32 occlusion_fraction(
        BSPItem const& bsp, Vecf3 const& listener, Vecf3 const& source)
    {
        Vecf3 const to_source = source - listener;
        f32 const   distance  = glm::length(to_source);
        if(distance < 0.2f)
            return 0.f;
        Vecf3 const dir   = to_source / distance;
        Vecf3       right = glm::cross(dir, Vecf3{0.f, 0.f, 1.f});
        right = glm::dot(right, right) < 1e-4f ? Vecf3{1.f, 0.f, 0.f}
                                               : glm::normalize(right);
        Vecf3 const up = glm::cross(right, dir);

        constexpr f32 source_spread   = 0.3f; /* ~1m */
        constexpr f32 listener_spread = 0.15f;
        std::array<Vecf3, 4> const offsets{right, -right, up, -up};

        u32  blocked{0};
        u32  total{0};
        auto cast = [&](Vecf3 const& from, Vecf3 const& to) {
            /* An end inside solid says nothing about the path between */
            if(!bsp.find_cluster_tree(from) || !bsp.find_cluster_tree(to))
                return;
            Vecf3 const span   = to - from;
            f32 const   length = glm::length(span);
            if(length < 0.2f)
                return;
            /* Stop short, sources tend to sit on a floor or in a wall */
            Vecf3 const end = from + span * ((length - 0.1f) / length);
            total++;
            if(bsp.raycast(from, end).has_value())
                blocked++;
        };
        cast(listener, source);
        for(auto const& offset : offsets)
            cast(listener, source + offset * source_spread);
        for(auto const& offset : offsets)
            cast(listener + offset * listener_spread, source);

        return total ? static_cast<f32>(blocked) / static_cast<f32>(total)
                     : 0.f;
    }

    void apply_occlusion(sound_unit_t& unit)
    {
        if(std::abs(unit.occlusion - unit.occlusion_applied) < 0.005f)
            return;
        unit.occlusion_applied = unit.occlusion;
        bool const  efx       = occlusion_uses_efx();
        auto const& tune      = occlusion_tuning;
        f32 const   occlusion = efx ? unit.occlusion : 0.f;
        f32 const   shaped    = std::pow(occlusion, tune.curve);
        /* The reverb only goes once the sound is properly enclosed */
        f32 const wet = std::pow(occlusion, tune.curve * 2.f);
        for(auto& track : unit.tracks)
        {
            if(efx && !track.filter)
            {
                track.filter = snd.alloc_filter();
                if(track.filter)
                    track.source->set_direct_filter(track.filter.get());
            }
            if(track.filter)
                track.filter->set_lowpass(
                    1.f - tune.dry_gain * shaped, 1.f - tune.dry_hf * occlusion);
            if(efx && unit.has_sends && !track.send_filter)
            {
                track.send_filter = snd.alloc_filter();
                if(track.send_filter)
                    for(u32 i : {0u, 1u})
                        track.source->set_send(
                            i,
                            environment.slots[i].get(),
                            track.send_filter.get());
            }
            if(track.send_filter)
                track.send_filter->set_lowpass(
                    1.f - tune.wet_gain * wet, 1.f - tune.dry_hf * wet);
            if(efx)
            {
                track.source->template set_property<
                    oaf::source_property::room_rolloff_factor>(
                    tune.room_rolloff);
                track.source->template set_property<
                    oaf::source_property::air_absorption_factor>(
                    tune.air_absorption);
            }
            set_gain(unit, track, track.base_gain);
        }
    }

    static Vecf3 portal_normal(BSPItem::Portal const& portal)
    {
        Vecf3       normal{0.f};
        auto const& v = portal.vertices;
        for(size_t i = 0; i < v.size(); i++)
            normal += glm::cross(v[i], v[(i + 1) % v.size()]);
        return glm::length(normal) > 1e-6f ? glm::normalize(normal) : normal;
    }

    /* Where the way from the listener to next goes through the portal */
    static Vecf3 portal_crossing(
        blam::bsp::cluster_portal const&   portal,
        sound_unit_t::portal_cost_t const& entry,
        Vecf3 const&                       from)
    {
        Vecf3 const centroid = portal.centroid;
        Vecf3 const n        = entry.normal;
        if(glm::dot(n, n) < 0.5f)
            return centroid;
        f32 const from_side = glm::dot(from - centroid, n);
        f32 const next_side = glm::dot(entry.next - centroid, n);
        Vecf3     point     = from - n * from_side;
        if(from_side * next_side < 0.f)
            point =
                glm::mix(from, entry.next, from_side / (from_side - next_side));
        /* The bounding disk, a little generous at the polygon's corners */
        Vecf3 radial = point - centroid;
        radial -= n * glm::dot(radial, n);
        f32 const length = glm::length(radial);
        f32 const radius = portal.bound_radius;
        if(length > radius)
            radial *= radius / length;
        return centroid + radial;
    }

    /* Shortest walk from the sound to every portal, between centroids */
    static auto find_portal_costs(
        BSPItem const& bsp, u32 from, Vecf3 const& from_pos)
    {
        using portal_t = blam::bsp::cluster_portal;
        std::unordered_map<portal_t const*, sound_unit_t::portal_cost_t> costs;
        using node_t = std::pair<f32, portal_t const*>;
        std::priority_queue<node_t, std::vector<node_t>, std::greater<>> open;

        auto relax =
            [&](u32 cluster, f32 base, Vecf3 const& at, portal_t const* via) {
                for(auto const& portal : bsp.clusters[cluster].portals)
                {
                    f32 const next =
                        base + glm::distance(at, portal.data->centroid);
                    auto [it, fresh] = costs.try_emplace(portal.data);
                    if(!fresh && next >= it->second.cost)
                        continue;
                    it->second = {next, base, at, portal_normal(portal), via};
                    open.push({next, portal.data});
                }
            };
        if(from >= bsp.clusters.size())
            return costs;
        relax(from, 0.f, from_pos, nullptr);
        while(!open.empty())
        {
            auto [cost, portal] = open.top();
            open.pop();
            if(cost > costs[portal].cost)
                continue;
            for(i32 side : {portal->front_cluster, portal->back_cluster})
                if(side >= 0 && static_cast<size_t>(side) < bsp.clusters.size())
                    relax(
                        static_cast<u32>(side), cost, portal->centroid, portal);
        }
        return costs;
    }

    /* The sound's point if it is in a cluster, else a nearby one that is */
    static std::optional<Vecf3> find_open_position(
        BSPItem const& bsp, Vecf3 const& position)
    {
        if(bsp.find_cluster_tree(position))
            return position;
        std::array<Vecf3, 14> directions{};
        u32                   n{0};
        for(i32 axis = 0; axis < 3; axis++)
            for(f32 sign : {1.f, -1.f})
            {
                Vecf3 d{0.f};
                d[axis]         = sign;
                directions[n++] = d;
            }
        for(f32 x : {1.f, -1.f})
            for(f32 y : {1.f, -1.f})
                for(f32 z : {1.f, -1.f})
                    directions[n++] = glm::normalize(Vecf3{x, y, z});
        for(f32 radius : {0.1f, 0.25f, 0.5f, 1.f})
            for(auto const& d : directions)
                if(bsp.find_cluster_tree(position + d * radius))
                    return position + d * radius;
        return std::nullopt;
    }

    void update_route(sound_unit_t& unit, BSPItem const* bsp)
    {
        /* Moving sounds only rebuild once they have gone some way */
        if(unit.route_bsp == bsp &&
           glm::distance(unit.route_origin, *unit.position) < 1.f)
            return;
        unit.route_bsp    = bsp;
        unit.route_origin = *unit.position;
        unit.open_position =
            bsp ? find_open_position(*bsp, *unit.position) : std::nullopt;
        unit.cluster = unit.open_position
                           ? bsp->find_cluster_tree(*unit.open_position)
                           : std::nullopt;
        unit.portal_costs.clear();
        if(unit.cluster)
            unit.portal_costs =
                find_portal_costs(*bsp, *unit.cluster, *unit.open_position);
    }

    /* Way out of the listener's cluster towards the sound. Portals that are
     * nearly as short pull too, so the direction never flips between them */
    std::optional<Vecf3> routed_position(
        sound_unit_t const& unit, BSPItem const& bsp, u32 listener) const
    {
        if(unit.portal_costs.empty() || !unit.cluster ||
           listener == *unit.cluster || listener >= bsp.clusters.size())
            return std::nullopt;
        auto const& portals = bsp.clusters[listener].portals;

        struct candidate_t
        {
            f32   total;
            Vecf3 crossing;
            Vecf3 next;
        };

        /* Each portal on the way is crossed where the line towards the
         * next one goes, so both sides of a portal agree on the length */
        auto candidate =
            [&](BSPItem::Portal const& portal) -> std::optional<candidate_t> {
            candidate_t result{0.f, {}, {}};
            Vecf3       at = listener_pos;
            Vecf3       end{};
            auto const* step = portal.data;
            for(u32 depth = 0; step; depth++)
            {
                auto it = unit.portal_costs.find(step);
                if(it == unit.portal_costs.end() || depth > 256)
                    return std::nullopt;
                Vecf3 const crossing = portal_crossing(*step, it->second, at);
                result.total += glm::distance(at, crossing);
                if(depth == 0)
                    result.crossing = crossing;
                if(depth == 1)
                    result.next = crossing;
                at   = crossing;
                end  = it->second.next;
                step = it->second.next_portal;
                if(depth == 0 && !step)
                    result.next = end;
            }
            result.total += glm::distance(at, end);
            return result;
        };
        f32 best = std::numeric_limits<f32>::max();
        for(auto const& portal : portals)
            if(auto c = candidate(portal))
                best = std::min(best, c->total);
        if(best == std::numeric_limits<f32>::max())
            return std::nullopt;

        f32 const spread = std::max(occlusion_tuning.portal_spread, 0.01f);
        Vecf3     direction{0.f};
        for(auto const& portal : portals)
        {
            auto c = candidate(portal);
            if(!c)
                continue;
            f32 const weight = std::exp(-(c->total - best) / spread);
            /* Close to a portal, listen through it to what comes after */
            f32 const   near  = glm::distance(listener_pos, c->crossing);
            Vecf3 const after = c->next - listener_pos;
            Vecf3       look =
                glm::length(after) > 1e-3f ? glm::normalize(after) : Vecf3{0.f};
            if(near > 1e-3f)
                look = glm::mix(
                    look,
                    (c->crossing - listener_pos) / near,
                    glm::smoothstep(0.f, 1.f, near));
            direction += look * weight;
        }
        if(glm::length(direction) < 1e-3f)
            return std::nullopt;
        return listener_pos + glm::normalize(direction) * best;
    }

    static Quatf rotation_between(Vecf3 const& from, Vecf3 const& to)
    {
        f32 const cos_angle = glm::dot(from, to);
        if(cos_angle < -0.9999f)
        {
            Vecf3 const other = std::abs(from.x) < 0.9f ? Vecf3{1.f, 0.f, 0.f}
                                                        : Vecf3{0.f, 1.f, 0.f};
            return glm::angleAxis(
                glm::pi<f32>(), glm::normalize(glm::cross(from, other)));
        }
        Vecf3 const axis = glm::cross(from, to);
        return glm::normalize(Quatf(1.f + cos_angle, axis.x, axis.y, axis.z));
    }

    /* Blend from the real position towards the route by how blocked the
     * direct path is, so a sound in plain view stays where it is */
    void apply_position(
        sound_unit_t&      unit,
        BSPItem const*     bsp,
        std::optional<u32> listener,
        f32                blend)
    {
        Vecf3 const real = *unit.position;
        Quatf       turn{1.f, 0.f, 0.f, 0.f};
        f32         extra{0.f};
        unit.route_distance = 0.f;
        std::optional<Vecf3> routed;
        if(portal_routing && bsp && listener && has_listener)
            routed = routed_position(unit, *bsp, *listener);
        Vecf3 const to_real   = real - listener_pos;
        f32 const   real_dist = glm::length(to_real);
        if(routed)
        {
            f32 const   weight     = unit.occlusion;
            Vecf3 const to_route   = *routed - listener_pos;
            f32 const   route_dist = glm::length(to_route);
            unit.route_distance    = route_dist;
            if(weight > 0.f && real_dist > 1e-3f && route_dist > 1e-3f)
            {
                Vecf3 const direction = glm::normalize(
                    glm::mix(
                        to_real / real_dist, to_route / route_dist, weight));
                turn  = rotation_between(to_real / real_dist, direction);
                extra = glm::mix(real_dist, route_dist, weight) - real_dist;
            }
        }
        /* Smoothed as a turn and a length, a lerped offset cuts corners */
        unit.route_turn = glm::slerp(unit.route_turn, turn, blend);
        unit.route_extra += (extra - unit.route_extra) * blend;
        Vecf3 const position =
            has_listener && real_dist > 1e-3f
                ? listener_pos + (unit.route_turn * (to_real / real_dist)) *
                                     (real_dist + unit.route_extra)
                : real;
        if(position == unit.position_applied)
            return;
        unit.position_applied = position;
        for(auto& track : unit.tracks)
            track.source->template set_property<oaf::source_property::position>(
                position);
    }

    void update_occlusion(Proxy& p, f32 dt)
    {
        BSPCache<Ver> const* bsp_cache{};
        p.subsystem(bsp_cache);
        BSPItem const* bsp = bsp_cache ? active_bsp(*bsp_cache) : nullptr;
        std::optional<u32> const listener_cluster =
            bsp && has_listener ? bsp->find_cluster_tree(listener_pos)
                                : std::nullopt;
        /* Outside the level (flycam) everything would read as occluded */
        bool const trace = occlusion_enabled && listener_cluster.has_value();
        f32 const  blend =
            std::min(1.f, dt / std::max(occlusion_tuning.smoothing, 1e-3f));
        bool const reapply =
            occlusion_uses_efx() != occlusion_efx_applied ||
            !(occlusion_tuning == occlusion_tuning_applied);
        occlusion_efx_applied    = occlusion_uses_efx();
        occlusion_tuning_applied = occlusion_tuning;

        auto update = [&](sound_unit_t& unit) {
            if(!unit.position)
                return;
            update_route(unit, bsp);
            f32 target = trace
                             ? occlusion_fraction(
                                   *bsp,
                                   listener_pos,
                                   unit.open_position.value_or(*unit.position))
                             : 0.f;
            if(target != unit.occlusion_target)
            {
                unit.occlusion_target = target;
                cDebug(
                    "Sound {} at {} occlusion {:.2f} from {}",
                    index.name_of(unit.source),
                    *unit.position,
                    target,
                    listener_pos);
            }
            unit.occlusion += (target - unit.occlusion) * blend;
            if(reapply)
                unit.occlusion_applied = -1.f;
            apply_occlusion(unit);
            apply_position(unit, bsp, listener_cluster, blend);
            apply_range(unit);
        };
        for(auto& [id, unit] : active_sounds)
            update(unit);
        for(auto& [id, unit] : fading_sounds)
            update(unit);
        for(auto& unit : singleshot_sounds)
            update(unit);
    }

    /* Both slots get a send, the slot gains decide which one is heard */
    void attach_environment(sound_unit_t& unit)
    {
        auto& env = environment;
        if(!env.slots[0] || !env.slots[1])
            return;
        unit.has_sends = true;
        for(auto& track : unit.tracks)
            for(u32 i : {0u, 1u})
                track.source->set_send(i, env.slots[i].get());
    }

    virtual void process(SoundEvent& ev, libc_types::c_ptr data) final
    {
        /* Immediate regardless of loading state. */
        if(ev.type == SoundEvent::clear_all)
        {
            active_sounds.clear();
            fading_sounds.clear();
            singleshot_sounds.clear();
            queued_events.clear();
            /* These point into the old map */
            environment.ref     = nullptr;
            environment.target  = 0.f;
            pending_bsp         = nullptr;
            has_pending_cluster = false;
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
            /* Nothing to move yet, and the next one supersedes it */
            if(ev.type == SoundEvent::update_sound)
                return;
            auto event = capture(ev, data);

            if(std::holds_alternative<std::monostate>(event.data))
                return;

            queued_events.push_back(std::move(event));
            return;
        }

        if(ev.type == SoundEvent::background_sound_transition)
        {
            auto const& trans =
                *reinterpret_cast<BackgroundSoundTransitionEvent const*>(data);
            transition_background(trans.sound, trans.sound_tag);
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
        if(ev.type == SoundEvent::update_sound)
        {
            auto const* update = reinterpret_cast<UpdateSoundEvent const*>(data);
            auto        it     = active_sounds.find(ev.entity_id);
            if(it == active_sounds.end())
                return;
            auto& unit = it->second;
            if(unit.position)
                unit.position = update->position;
            unit.gain_scale = update->gain;
            for(auto& track : unit.tracks)
                if(track.source)
                    track.source->template set_property<
                        oaf::source_property::pitch>(update->pitch);
            apply_volume(unit, (*sound_cache.find(unit.index)).second);
            return;
        }
        if(ev.type == SoundEvent::play_sound)
        {
            auto const& play  = reinterpret_cast<PlaySoundEvent const*>(data);
            auto* ref = play->sound;
            blam::tagref_t ref_{};
            // When we want to use sounds not sourced by tagref
            if(play->sound_tag)
            {
                ref_ = play->sound_tag->as_ref();
                ref = &ref_;
            }
            if(!ref)
            {
                cWarning("No ref for PlaySoundEvent");
                return;
            }
            if(play->looping)
            {
                /* Same entity again (BSP section reload) keeps it running */
                auto it = active_sounds.find(ev.entity_id);
                if(it != active_sounds.end() &&
                   it->second.source.tag_id == ref->tag_id)
                    return;
            }
            cDebug("Queueing PlaySoundEvent for {}", index.name_of(*ref));
            auto unit = make_sound_unit(*ref, LoopSoundEvent::usage_t::general);
            if(!unit.index.valid())
                return;
            for(auto& track : unit.tracks)
            {
                track.source->template set_property<
                    oaf::source_property::relative>(play->relative);
                track.source->template set_property<
                    oaf::source_property::position>(play->position);
                track.source->spatialize_as(oaf::source_t::always);
            }
            if(!play->relative)
            {
                unit.position         = play->position;
                unit.position_applied = play->position;
                attach_environment(unit);
            }
            if(play->looping)
                active_sounds[ev.entity_id] = std::move(unit);
            else
                singleshot_sounds.push_back(std::move(unit));
        }
    }
};

#if defined(FEATURE_ENABLE_ImGui)

using SoundUIManifest = compo::SubsystemManifest<
    empty_list_t,
    type_list_t<
        LoadingStatus,
        SoundSystem<halo_version>,
        SoundCache<halo_version>>,
    empty_list_t>;

struct SoundUISystem
    : compo::RestrictedSubsystem<SoundUISystem, SoundUIManifest>
{
    using type  = SoundUISystem;
    using Proxy = compo::proxy_of<SoundUIManifest>;

    SoundUISystem()
    {
        this->priority = 2048;
    }

    bool main_thread_only() const override
    {
        return true;
    }

    void start_restricted(Proxy& p, compo::time_point const&)
    {
#if defined(FEATURE_ENABLE_ImGui)
        auto& sound_cache = p.subsystem<SoundCache<halo_version>>();
        auto& snd         = p.subsystem<SoundSystem<halo_version>>();
        auto& loading     = p.subsystem<LoadingStatus>();

        if(loading.loading)
            return;

        if(ImGui::Begin("Sound"))
        {
            if(ImGui::BeginTabBar("AudioTabs"))
            {
                if(ImGui::BeginTabItem("Tracks"))
                {
                    ImGui::Text(
                        "active=%zu fading=%zu queued=%zu sounds_loaded=%d",
                        snd.active_sounds.size(),
                        snd.fading_sounds.size(),
                        snd.queued_events.size(),
                        (int)(loading.loaded_sounds == LoadingStatus::loaded));
                    ImGui::Text(
                        "lifetime: bg_transitions=%u (with_sound=%u) "
                        "started=%u replayed=%u",
                        snd.stat_bg_total,
                        snd.stat_bg_with_snd,
                        snd.stat_started,
                        snd.stat_replayed);
                    {
                        auto const& env = snd.environment;
                        std::string_view env_name =
                            env.ref ? snd.index.name_of(*env.ref) : "none";
                        ImGui::Text(
                            "environment: %.*s efx=%s gains=%.2f/%.2f",
                            static_cast<int>(env_name.size()),
                            env_name.data(),
                            env.slots[0] ? "yes" : "no",
                            env.gain[0],
                            env.gain[1]);
                        ImGui::Checkbox("Reverb", &snd.environment.enabled);
                        ImGui::SameLine();
                        ImGui::Checkbox("Occlusion", &snd.occlusion_enabled);
                        ImGui::SameLine();
                        ImGui::Checkbox(
                            "Gain-only occlusion", &snd.occlusion_gain_only);
                        ImGui::SameLine();
                        ImGui::Checkbox("Portal routing", &snd.portal_routing);
                        auto& tune = snd.occlusion_tuning;
                        ImGui::SliderFloat("Dry gain cut", &tune.dry_gain, 0.f, 1.f);
                        ImGui::SliderFloat("Dry HF cut", &tune.dry_hf, 0.f, 1.f);
                        ImGui::SliderFloat("Reverb cut", &tune.wet_gain, 0.f, 1.f);
                        ImGui::SliderFloat("Gain-only cut", &tune.gain_only, 0.f, 1.f);
                        ImGui::SliderFloat("Curve", &tune.curve, 0.5f, 4.f);
                        ImGui::SliderFloat(
                            "Reverb rolloff", &tune.room_rolloff, 0.f, 2.f);
                        ImGui::SliderFloat(
                            "Air absorption", &tune.air_absorption, 0.f, 10.f);
                        ImGui::SliderFloat(
                            "Smoothing", &tune.smoothing, 0.f, 1.f);
                        ImGui::SliderFloat(
                            "Portal spread", &tune.portal_spread, 0.f, 10.f);
                    }
                    ImGui::Separator();
                    auto sound_row = [&](u64                 entity,
                                         sound_unit_t const& sound,
                                         ImVec4              color) {
                        auto item_it = sound_cache.find(sound.index);
                        auto name    = snd.index.name_of(sound.source);
                        ImGui::TextColored(
                            color,
                            "[sound/%04lu] %.*s",
                            entity,
                            static_cast<int>(name.size()),
                            name.data());
                        ImGui::Text("    [volume] %f", sound.volume);
                        if(sound.position)
                            ImGui::Text(
                                "    [occlusion] %.2f (target %.2f)",
                                sound.occlusion,
                                sound.occlusion_target);
                        if(sound.route_distance > 0.f)
                            ImGui::Text(
                                "    [route] %.1f via portals, %zu costed",
                                sound.route_distance,
                                sound.portal_costs.size());
                        if(item_it == sound_cache.end())
                        {
                            ImGui::Text("    [not in cache]");
                            return;
                        }
                        SoundItem const& item = item_it->second;
                        u32              track_i{0};
                        for(auto const& track : sound.tracks)
                        {
                            auto role =
                                magic_enum::enum_name(track.active.role);
                            std::string_view name{"[?]"};
                            if(track_i < item.tracks.size())
                            {
                                auto const& track_ = item.tracks[track_i];
                                auto        sit =
                                    track_.sounds.find(track.active.role);
                                if(sit != track_.sounds.end())
                                {
                                    auto const& [sound_tag, _, __] =
                                        sit->second;
                                    if(sound_tag)
                                        name = snd.index.name_of(*sound_tag);
                                }
                            }
                            ++track_i;
                            ImGui::Text(
                                "    [track/%02u/%02u/%.*s] %.*s ",
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
                                for(auto const& [role, sound] : dsound.sounds)
                                {
                                    auto const& [sound_tag, _, __] = sound;
                                    std::string_view name{"[?]"};
                                    if(sound_tag)
                                        name = snd.index.name_of(*sound_tag);
                                    auto role_name =
                                        magic_enum::enum_name(role);
                                    ImGui::Text(
                                        "      [detail/%.*s] %.*s",
                                        static_cast<int>(role_name.size()),
                                        role_name.data(),
                                        static_cast<int>(name.size()),
                                        name.data());
                                }
                            }
                        }
                    };
                    for(auto const& [entity, sound] : snd.active_sounds)
                        sound_row(entity, sound, ImVec4(0, 1, 0, 1));
                    for(auto const& [entity, sound] : snd.fading_sounds)
                        sound_row(entity, sound, ImVec4(0.7, 0.5, 0, 1));
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("Testing"))
                {
                    auto& voice   = snd.voice;
                    auto& buffers = snd.buffers;
                    auto& sources = snd.sources;

                    ImGui::InputText("Voice", voice.voice, sizeof(voice.voice));
                    ImGui::InputText(
                        "Backend", voice.backend, sizeof(voice.backend));
                    ImGui::InputText(
                        "Phrase", voice.phrase, sizeof(voice.phrase));
                    if(ImGui::Button("Play voice"))
                    {
                        auto src = net::MkUrl(
                            fmt::format(
                                "http://10.0.0.17:9006/{}"
                                "/v1/audio/speech"
                                "?input={}&voice={}",
                                voice.backend,
                                stl_types::str::url_encode::encode(
                                    voice.phrase),
                                voice.voice));
                        net::Resource sound(net::create_curl_context(), src);

                        if(!sound.fetch().has_value() &&
                           sound.data().has_value())
                        {
                            buffers.push_back(snd.snd.alloc_buffer());
                            sources.push_back(snd.snd.alloc_source());
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
};

#endif

#endif

void alloc_sound_system(compo::EntityContainer& e, bool enabled)
{
    e.register_subsystem_inplace<SoundPreferences>();
    if(!enabled)
        return;
#if defined(FEATURE_ENABLE_OAF)
    ProfContext _;
    auto& sound_sys = e.register_subsystem_inplace<SoundSystem<halo_version>>(
        std::ref(e.subsystem_cast<oaf::system>()),
        std::ref(e.subsystem_cast<SoundCache<halo_version>>()),
        &e.subsystem_cast<LoadingStatus>(),
        std::ref(e.subsystem_cast<GameEventBus>()));
    e.register_subsystem_services<SoundSystem<halo_version>>(&sound_sys);
#if defined(FEATURE_ENABLE_ImGui)
    e.register_subsystem_inplace<SoundUISystem>();
#endif
#endif
}
