#include "h3_sound.h"

#if defined(FEATURE_ENABLE_OAF)
#include <oaf/api.h>
#if defined(OAF_XMA2_DECODER_ENABLED)
#include <oaf/xma2/xma2_decode.h>
#endif
#endif

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>
#include <coffee/core/CDebug>

namespace h3 {

using H3SoundManifest = compo::SubsystemManifest<
    compo::empty_list_t,
    compo::type_list_t<const H3SoundBank>,
    compo::empty_list_t>;

/* Plays the sound bank: for now the background music, as a source that
 * runs the intro once and then loops over the loop permutations */
struct ElegySoundSystem : compo::RestrictedSubsystem<ElegySoundSystem, H3SoundManifest>
{
    using Proxy = compo::proxy_of<H3SoundManifest>;
    using type  = ElegySoundSystem;

#if defined(FEATURE_ENABLE_OAF)
    ElegySoundSystem(oaf::api* api)
        : m_audio(api)
    {
        priority = 3100;
    }
#else
    ElegySoundSystem(void*)
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
        auto const& bank = p.template subsystem<H3SoundBank>();
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
    std::shared_ptr<oaf::buffer_t> upload(oaf::api& api, H3Sound const& sound)
    {
        if(sound.data.empty())
            return nullptr;
#if defined(OAF_XMA2_DECODER_ENABLED)
        auto                       buffer = api.alloc_buffer();
        oaf::decode::xma2::decoder xma2;
        if(!xma2.decode(
               gsl::span<const char>(sound.data),
               {.channels = sound.channels, .rate = sound.rate, .samples = sound.samples},
               *buffer))
        {
            cWarning("H3: sound {}: XMA2 decode failed", sound.name);
            return nullptr;
        }
        return buffer;
#else
        cWarning("H3: sound {}: XMA2, no decoder in this build", sound.name);
        return nullptr;
#endif
    }
#endif

    void play_music(H3LoopingSound const& music)
    {
#if defined(FEATURE_ENABLE_OAF)
        auto* api = m_audio;
        if(!api || music.loops.empty())
            return;
        m_music.source = api->alloc_source();
        m_music.source->template set_property<oaf::source_property::relative>(true);
        m_music.source->template set_property<oaf::source_property::gain>(music.gain);
        m_music.source->spatialize_as(oaf::source_t::never);
        m_music.intro = upload(*api, music.in);
        for(auto const& loop : music.loops)
            if(auto buffer = upload(*api, loop))
                m_music.loops.push_back(std::move(buffer));
        if(m_music.loops.empty())
        {
            m_music = {};
            return;
        }
        if(m_music.intro)
            m_music.source->queue(*m_music.intro);
        else
            m_music.source->template set_property<oaf::source_property::looping>(true);
        for(auto const& loop : m_music.loops)
            m_music.source->queue(*loop);
        cDebug(
            "H3: sound: {} playing, intro + {} loop permutations, {} Hz {}ch",
            music.name,
            m_music.loops.size(),
            music.loops.front().rate,
            music.loops.front().channels);
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
        std::shared_ptr<oaf::source_t>              source;
        std::shared_ptr<oaf::buffer_t>              intro;
        std::vector<std::shared_ptr<oaf::buffer_t>> loops;
    } m_music;
#endif
};

void alloc_h3_sound(compo::EntityContainer& e, void* api)
{
#if defined(FEATURE_ENABLE_OAF)
    e.register_subsystem_inplace<ElegySoundSystem>(static_cast<oaf::api*>(api));
#else
    e.register_subsystem_inplace<ElegySoundSystem>(api);
#endif
}

} // namespace h3
