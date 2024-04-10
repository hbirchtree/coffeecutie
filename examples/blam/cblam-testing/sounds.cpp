#include "sounds.h"

#include "components.h"
#include "selected_version.h"
#include "sound_cache.h"

#include <deque>
#include <oaf/api_system.h>
#include <oaf/ogg/ogg_decode.h>
#include <peripherals/stl/enumerate.h>
#include <ranges>

#if defined(OAF_IMA_DECODER_ENABLED)
#include <oaf/ima_adpcm/decode.h>
#endif

using Coffee::Logging::cDebug;

using SoundManifest = compo::SubsystemManifest<
    type_list_t<SoundEffects>,
    type_list_t<LoadingStatus>,
    empty_list_t>;

enum class status_t
{
    unplayed,
    queued,
    played,
};

struct track_t
{
    std::shared_ptr<oaf::source_t> source;

    struct entry_t
    {
        std::shared_ptr<oaf::buffer_t> buffer;
        bool                           looping{false};
        status_t                       status{status_t::unplayed};
        blam::sound::sound const*      sound{nullptr};
    };

    std::vector<entry_t> buffers;
    size_t               position{0};
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

    std::map<u64, track_t> active_tracks;

    struct queued_event_t
    {
        SoundEvent                                   event;
        std::variant<LoopSoundEvent, PlaySoundEvent> data;
    };

    std::vector<queued_event_t> queued_events;

    void start_restricted(Proxy& p, compo::time_point const& t)
    {
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
                default:
                    break;
                }
            }
            queued_events.clear();
        }

        for(auto& [id, track] : active_tracks)
        {
            auto&              src = track.source;
            std::deque<size_t> deletions;
            for(auto& [i, buffer] : stl_types::enumerate(track.buffers))
            {
                if(i < track.position)
                    continue;
                if(buffer.status == status_t::unplayed)
                {
                    src->queue(*buffer.buffer);
                    src->template set_property<oaf::source_property::gain>(
                        buffer.sound->gain_modifier);
                    // cDebug("Queueing {}", i);
                    buffer.status  = status_t::queued;
                    track.position = i;
                    break;
                }
                if(buffer.status == status_t::queued)
                {
                    auto [queued, processed] = src->buffer_queue();
                    if(queued != processed)
                    {
                        // cDebug("Waiting for {} to play", i);
                        track.position = i;
                        break;
                    }
                    // cDebug("Finished playing {}", i);
                    src->unqueue(*buffer.buffer);
                    buffer.status =
                        buffer.looping ? status_t::unplayed : status_t::played;
                    track.position++;
                }
                if(buffer.status == status_t::played)
                {
                    // cDebug("Cleaning up {}", i);
                    buffer.buffer.reset();
                    deletions.push_back(i);
                    continue;
                }
            }
            if(track.position == track.buffers.size())
                track.position = 0;
            for(auto const& i : deletions | std::views::reverse)
                track.buffers.erase(track.buffers.begin() + i);
        }
    }

    void end_restricted(Proxy&, compo::time_point const&)
    {
    }

    void decode_audio(
        sound_ptr const&               sound_,
        std::vector<track_t::entry_t>& buffers,
        track_t::entry_t const&        props)
    {
        auto const& [tag, sound, heap] = sound_;
        auto ranges = *sound->pitch_ranges(heap);
        if(ranges.empty())
            return;
        auto perms = *ranges[0].permutations(heap);
        if(perms.empty())
            return;

        blam::sound::pitch_permutation_t const* perm = &perms[0];
        using sound_t                                = blam::sound::sound;

        while(perm)
        {
            auto data_ = index.deref(*tag, perm->sample_data());
            if(!data_.has_value())
                break;
            auto data = data_.value();
            switch(sound->codec)
            {
            case blam::sound::sound::codec_t::ogg: {
                auto                      buf = snd.alloc_buffer();
                oaf::decode::ogg::decoder decoder;
                decoder.decode(data, {}, {}, *buf);
                buffers.push_back({
                    .buffer  = buf,
                    .looping = props.looping,
                    .sound   = sound,
                });
                break;
            }
            case blam::sound::sound::codec_t::ima_adpcm:
            case blam::sound::sound::codec_t::xbox_adpcm: {
                auto          buf = snd.alloc_buffer();
                oaf::format_t fmt;
                fmt.format = oaf::Format::ima_adpcm;
                fmt.frequency =
                    sound->sample_rate == sound_t::_22kHz ? 22050 : 44100;
                fmt.channels = sound->channels == sound_t::mono ? 1 : 2;
                if(snd.formats().ima4_adpcm)
                {
                    buf->upload(data, fmt);
                }
#if defined(OAF_IMA_DECODER_ENABLED)
                else
                {
                    oaf::decode::ima_adpcm::decoder decoder;
                    decoder.decode(data, fmt, *buf);
                }
#endif
                buffers.push_back({
                    .buffer  = buf,
                    .looping = props.looping,
                    .sound   = sound,
                });
                break;
            }
            default:
                break;
            }
            if(perm->next_permutation_idx == -1)
                break;
            perm = &perms[perm->next_permutation_idx];
        }
    }

    virtual void process(SoundEvent& ev, libc_types::c_ptr data) final
    {
        if(ev.type == SoundEvent::clear_all)
            active_tracks.clear();

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
            default:
                return;
            }
            queued_events.push_back(event);
            return;
        }

        if(ev.type == SoundEvent::loop_sound)
        {
            auto const& loop  = reinterpret_cast<LoopSoundEvent const*>(data);
            auto        sound = sound_cache.predict(*loop->sound);
            if(!sound.valid())
                return;
            SoundItem const& item  = (*sound_cache.find(sound)).second;
            auto const&      track = item.tracks.front();
            auto start_ = track.sounds.find(SoundItem::role_t::start);
            auto loop_  = track.sounds.find(SoundItem::role_t::loop);
            /* Custom maps apparently do this thing... */
            if(loop_ == track.sounds.end())
                return;
            std::vector<track_t::entry_t> buffers;
            if(start_ != track.sounds.end())
                decode_audio(start_->second, buffers, track_t::entry_t{});
            decode_audio(
                loop_->second,
                buffers,
                track_t::entry_t{
                    .looping = true,
                });
            cDebug("Queueing {} audio buffers", buffers.size());
            active_tracks.emplace(
                ev.entity_id,
                track_t{
                    .source  = snd.alloc_source(),
                    .buffers = std::move(buffers),
                });
        }
        if(ev.type == SoundEvent::play_sound)
        {
            auto const& play = reinterpret_cast<PlaySoundEvent const*>(data);
        }
    }
};

void alloc_sound_system(compo::EntityContainer& e)
{
    auto& sound_sys = e.register_subsystem_inplace<SoundSystem<halo_version>>(
        std::ref(e.subsystem_cast<oaf::system>()),
        std::ref(e.subsystem_cast<SoundCache<halo_version>>()),
        &e.subsystem_cast<LoadingStatus>());
    e.register_subsystem_services<SoundSystem<halo_version>>(&sound_sys);
}
