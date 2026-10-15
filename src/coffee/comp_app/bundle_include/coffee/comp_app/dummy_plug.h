#pragma once

#include <atomic>
#include <functional>
#include <optional>

#include <coffee/comp_app/subsystems.h>
#include <nlohmann/json.hpp>
#include <peripherals/concepts/sound_api.h>

namespace comp_app::dummy_plug {

using audio_format_t = semantic::concepts::sound::Format::format_t;

struct Config : comp_app::Config<Config>
{
    struct
    {
        libc_types::u32 frequency{44100};
        libc_types::u16 channels{2};
        audio_format_t  format{audio_format_t::pcm};
        libc_types::u16 bits{16};
    } audio_config;

    nlohmann::json         graphics_config{};
    nlohmann::json         config{};
    libc_types::u64        frame_index{0};
    compo::clock::duration frame_delta{std::chrono::microseconds{16667}};
    bool                   enabled{false};
    std::string            swrender{"none"};
    int                    screenshot_quality{30};

    struct pending_action_t
    {
        std::string     screenshot;
        std::string     event;
        nlohmann::json  data;
        libc_types::u32 settle;
    };

    std::vector<pending_action_t> pending_actions{};
    bool                          screenshot_armed{false};
    bool                          step_between_captures{false};
    bool                          defer_to_swap{false};

    /*! "time_base": "session": event times count on a clock the app shares
     *  between processes (e.g. a network session), once it reports one;
     *  events with "clock": "local" keep this process's own, for those that
     *  start the session. Called from any thread. */
    std::function<std::optional<compo::clock::duration>()> session_clock{};
    bool                                                   session{false};

    /*! The plug's clock: frames x frame_delta, or session time once known.
     *  Never runs backwards. */
    compo::clock::duration now()
    {
        compo::clock::duration out =
            frame_delta * static_cast<libc_types::i64>(frame_index);
        if(session && session_clock)
            if(auto at = session_clock())
                out = *at;
        auto last = m_last_now.load();
        while(out > last && !m_last_now.compare_exchange_weak(last, out))
            ;
        return std::max(out, last);
    }

  private:
    std::atomic<compo::clock::duration> m_last_now{};
};

/* Free-form dummy events we can insert and interpret per-application */
struct DummyEvent
{
    std::string    event;
    nlohmann::json data;
};

using DummyEventBus = BasicEventBus<DummyEvent>;

void fork_dummy_plugs(
    AppLoadableService::entity_container& container,
    dummy_plug::Config&                   dummy_plug);

void insert_dummy_plug(
    AppLoadableService::entity_container& container, Config& dummy_plug);

} // namespace comp_app::dummy_plug
