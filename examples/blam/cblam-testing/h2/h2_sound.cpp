#include "h2_sound.h"

#if defined(FEATURE_ENABLE_OAF)
#include <oaf/api.h>
#if defined(OAF_WMA_DECODER_ENABLED)
#include <oaf/wma/wma_decode.h>
#endif
#endif

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>
#include <coffee/core/CDebug>

namespace h2 {

using H2SoundManifest = compo::SubsystemManifest<
    compo::empty_list_t,
    compo::type_list_t<const H2SoundBank>,
    compo::empty_list_t>;

/* Plays the sound bank: for now the background music, as a source that
 * runs the intro once and then loops */
struct DimeterSoundSystem
    : compo::RestrictedSubsystem<DimeterSoundSystem, H2SoundManifest>
{
    using Proxy = compo::proxy_of<H2SoundManifest>;
    using type  = DimeterSoundSystem;

#if defined(FEATURE_ENABLE_OAF)
    DimeterSoundSystem(oaf::api* api)
        : m_audio(api)
    {
        priority = 3100;
    }
#else
    DimeterSoundSystem(void*)
    {
        priority = 3100;
    }
#endif

    bool main_thread_only() const override
    {
        return true;
    }

    void start_restricted(Proxy& p, time_point const&)
    {
        auto const& bank = p.template subsystem<H2SoundBank>();
        if(bank.generation != m_generation)
        {
            m_generation = bank.generation;
            stop_music();
            if(bank.loaded)
                play_music(bank.music);
        }
        update_music();
    }

  private:
#if defined(FEATURE_ENABLE_OAF)
    std::shared_ptr<oaf::buffer_t> upload(oaf::api& api, H2Sound const& sound)
    {
        if(sound.blocks.empty())
            return nullptr;
        if(sound.wma)
        {
#if defined(OAF_WMA_DECODER_ENABLED)
            auto                        buffer = api.alloc_buffer();
            oaf::decode::wma::decoder   wma;
            if(!wma.decode(gsl::span<const char>(sound.blocks), *buffer))
            {
                cWarning("H2: sound {}: WMA decode failed", sound.name);
                return nullptr;
            }
            return buffer;
#else
            cWarning("H2: sound {}: WMA, no decoder in this build", sound.name);
            return nullptr;
#endif
        }
        oaf::format_t fmt;
        fmt.format    = oaf::Format::ima_adpcm;
        fmt.frequency = sound.rate;
        fmt.channels  = sound.channels;
        fmt.bits      = 4;
        auto buffer   = api.alloc_buffer();
        buffer->upload(gsl::span<const char>(sound.blocks), fmt);
        return buffer;
    }
#endif

    void play_music(H2LoopingSound const& music)
    {
#if defined(FEATURE_ENABLE_OAF)
        auto* api = m_audio;
        if(!api || music.loop.blocks.empty())
            return;
        if(!music.loop.wma && !api->formats().ima4_adpcm)
        {
            cWarning("H2: sound: music needs IMA4 buffers, none here");
            return;
        }
        m_music.source = api->alloc_source();
        m_music.source->template set_property<oaf::source_property::relative>(true);
        m_music.source->template set_property<oaf::source_property::gain>(music.gain);
        m_music.source->spatialize_as(oaf::source_t::never);
        m_music.intro = upload(*api, music.in);
        m_music.loop  = upload(*api, music.loop);
        if(!m_music.loop)
        {
            m_music = {};
            return;
        }
        if(m_music.intro)
            m_music.source->queue(*m_music.intro);
        else
            m_music.source->template set_property<oaf::source_property::looping>(true);
        m_music.source->queue(*m_music.loop);
        cDebug(
            "H2: sound: {} playing, intro {} + loop {} bytes, {} Hz {}ch",
            music.name,
            music.in.blocks.size(),
            music.loop.blocks.size(),
            music.loop.rate,
            music.loop.channels);
#else
        (void)music;
#endif
    }

    /* Once the intro has played the source loops on the rest */
    void update_music()
    {
#if defined(FEATURE_ENABLE_OAF)
        if(!m_music.source || !m_music.intro)
            return;
        auto [queued, processed] = m_music.source->buffer_queue();
        if(processed < 1)
            return;
        m_music.source->unqueue(*m_music.intro);
        m_music.intro.reset();
        m_music.source->template set_property<oaf::source_property::looping>(true);
#endif
    }

    void stop_music()
    {
#if defined(FEATURE_ENABLE_OAF)
        m_music = {};
#endif
    }

    u32 m_generation{0};
#if defined(FEATURE_ENABLE_OAF)
    oaf::api* m_audio{nullptr};
    struct
    {
        std::shared_ptr<oaf::source_t> source;
        std::shared_ptr<oaf::buffer_t> intro, loop;
    } m_music;
#endif
};

void alloc_h2_sound(compo::EntityContainer& e, void* api)
{
#if defined(FEATURE_ENABLE_OAF)
    e.register_subsystem_inplace<DimeterSoundSystem>(static_cast<oaf::api*>(api));
#else
    e.register_subsystem_inplace<DimeterSoundSystem>(api);
#endif
}

} // namespace h2
