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

#include "../world/looping_music.h"

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
        auto                                         intro = upload(*api, music.in);
        std::vector<world::LoopingMusic::buffer_ptr> loops;
        for(auto const& loop : music.loops)
            if(auto buffer = upload(*api, loop))
                loops.push_back(std::move(buffer));
        if(!m_music.play(*api, music.gain, std::move(intro), std::move(loops)))
            return;
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

    void update_music()
    {
#if defined(FEATURE_ENABLE_OAF)
        m_music.update();
#endif
    }

    void stop_music()
    {
#if defined(FEATURE_ENABLE_OAF)
        m_music.stop();
#endif
    }

    u32 m_generation{0};
#if defined(FEATURE_ENABLE_OAF)
    oaf::api* m_audio{nullptr};
    world::LoopingMusic m_music;
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
