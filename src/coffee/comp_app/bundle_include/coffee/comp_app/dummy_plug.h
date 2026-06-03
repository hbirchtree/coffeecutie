#pragma once

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
};

/* Free-form dummy events we can insert and interpret per-application */
struct DummyEvent
{
    std::string event;
    nlohmann::json data;
};

using DummyEventBus = BasicEventBus<DummyEvent>;

void fork_dummy_plugs(
    AppLoadableService::entity_container& container,
    dummy_plug::Config&                   dummy_plug);

void insert_dummy_plug(
    AppLoadableService::entity_container& container, Config& dummy_plug);

} // namespace comp_app::dummy_plug
