#include <coffee/comp_app/dummy_plug.h>

#include <coffee/comp_app/app_events.h>
#include <coffee/comp_app/gl_config.h>
#include <coffee/core/debug/formatting.h>
#include <coffee/core/files/cfiles.h>
#include <coffee/core/task_queue/task.h>
#include <magic_enum.hpp>
#include <peripherals/stl/enumerate.h>

#if defined(FEATURE_ENABLE_OSMesaComponent)
#include <peripherals/posix/process.h>
#endif

using Coffee::cDebug;
using Coffee::Input::CIEvent;

namespace comp_app::dummy_plug {

enum class type_t
{
    none,
    screenshot,

    key,
    mouse_button,
    mouse_move,

    controller_connect,
    controller_button,
    controller_axis,
};

namespace {

void queue_input_event(
    comp_app::BasicEventBus<Coffee::Input::CIEvent>* bus,
    type_t                                           type,
    nlohmann::json const&                            event)
{
    using Coffee::Input::CIControllerAtomicUpdateEvent;
    using Coffee::Input::CIKeyEvent;
    using Coffee::Input::CIMouseButtonEvent;
    using Coffee::Input::CIMouseMoveEvent;

    CIEvent    ievent;
    const auto start_time = std::chrono::milliseconds(event.value("time", 0u));
    const auto emit_future_event =
        [bus]<typename T>(compo::duration delay, CIEvent event, T data) {
            rq::runtime_queue::QueueShot(
                rq::runtime_queue::GetCurrentQueue().value(),
                delay,
                [bus, event, data]() mutable {
                    bus->inject(event, &data);
                    cDebug(
                        "Injecting synthetic event: {}",
                        magic_enum::enum_name(event.type));
                })
                .value();
        };

    switch(type)
    {
    case type_t::controller_axis: {
        break;
    }
    case type_t::controller_button: {
        break;
    }
    case type_t::controller_connect: {
        ievent.type = CIEvent::ControllerUpdate;
        CIControllerAtomicUpdateEvent controller;
        controller.connected  = true;
        controller.controller = event.value("index", 0u);
        emit_future_event(start_time, ievent, controller);
        break;
    }
    case type_t::key: {
        using Coffee::Input::Latin1Mapping;
        using namespace Coffee::Input;

        ievent.type = CIEvent::Keyboard;
        CIKeyEvent key;
        key.key = magic_enum::enum_cast<Latin1Mapping>(
                      event.value("key", std::string()))
                      .value_or(CK_Null);
        key.mod = CIKeyEvent::PressedModifier;
        emit_future_event(start_time, ievent, key);
        auto unpress_delay =
            std::chrono::milliseconds(event.value("duration", 0u));
        key.mod = CIKeyEvent::NoneModifier;
        emit_future_event(start_time + unpress_delay, ievent, key);
        break;
    }
    default:
        break;
    }
}

} // namespace

void fork_dummy_plugs(
    AppLoadableService::entity_container& container,
    dummy_plug::Config&                   dummy_plug)
{
    Coffee::DProfContext _;

    cDebug("Dummy plug activated");

    auto config_file = platform::url::constructors::MkUrl(
        platform::env::var("DUMMY_PLUG_CONFIG").value());
    auto config_content = Coffee::Resource(config_file);
    if(!Coffee::FileMap(config_content))
        return;

    if(config_content.data().empty())
    {
        Coffee::Logging::cFatal("Dummy plug config file is empty");
    }

    dummy_plug.config  = nlohmann::json::parse(config_content.data());
    auto const& config = dummy_plug.config;

    if(config.empty())
    {
        Coffee::Logging::cFatal("Dummy plug config contains nothing");
    }

#if defined(FEATURE_ENABLE_OSMesaComponent)
    if(!config.contains("graphics"))
        return;

    struct version_t
    {
        libc_types::u32   major{}, minor{};
        GLConfig::Profile profile{GLConfig::Profile::Core};
        libc_types::u32   depth{32};
    };

    std::vector<version_t> versions;

    auto const& graphics = config["graphics"];
    if(graphics.contains("gl"))
        for(auto const& version : graphics["gl"])
        {
            versions.push_back({
                .major   = version.value("major", 0u),
                .minor   = version.value("minor", 0u),
                .profile = GLConfig::Core,
            });
        }
    if(graphics.contains("gles"))
        for(auto const& version : graphics["gles"])
        {
            versions.push_back({
                .major   = version.value("major", 0u),
                .minor   = version.value("minor", 0u),
                .profile = GLConfig::Embedded,
            });
            if(versions.back().major == 2)
                versions.back().depth = 16;
        }

    using platform::url::constructors::MkUrl;
    using semantic::RSCA;

    using namespace platform::common::posix;

    // We're gonna do some funny business here; we'll fork the process into X
    // amount of processes, each with their own GL config, all running in
    // software mode. It's gonna be great!
    std::vector<pid_t> children;
    auto& glConfig = container.service<AppLoader>()->config<GLConfig>();
    for(auto const& version : versions)
    {
        if(version.profile == GLConfig::Core)
        {
            glConfig.version.major = version.major;
            glConfig.version.minor = version.minor;
        } else
        {
            glConfig.version.major = 4;
            glConfig.version.minor = 5;
        }
        glConfig.profile = version.profile;
        glConfig.depthFmt =
            version.depth == 32 ? pix_fmt::Depth32 : pix_fmt::Depth16;

        pid_t    child_pid{};
        posix_ec ec;
        if(auto res = proc::fork(child_pid, ec);
           res == proc::fork_process::error)
        {
            Coffee::Logging::cFatal("Failed to fork: {}", ec.error_message);
            C_BREAK();
        } else if(res == proc::fork_process::child)
        {
            dummy_plug.graphics_config = nlohmann::json();
            dummy_plug.graphics_config["profile"] =
                magic_enum::enum_name(version.profile);
            dummy_plug.graphics_config["major"] = version.major;
            dummy_plug.graphics_config["minor"] = version.minor;

            platform::url::overrideSystemPath(
                RSCA::TempFile,
                MkUrl(*MkUrl(fmt::format("{}", getpid()), RSCA::TempFile)));
            platform::url::overrideSystemPath(
                RSCA::CachedFile,
                MkUrl(*MkUrl(fmt::format("{}", getpid()), RSCA::CachedFile)));
            auto* app_data             = platform::state->GetAppData().get();
            app_data->application_name = fmt::format(
                "{} (GL {} {}.{})",
                app_data->application_name,
                magic_enum::enum_name(version.profile),
                version.major,
                version.minor);
            // in the forked process, keep running as usual :)
            return;
        } else
            children.push_back(child_pid);
    }

    if(!children.empty())
    {
        posix_ec         ec;
        std::vector<int> results;
        for(auto [i, child] : stl_types::enumerate(children))
        {
            results.push_back(0);
            proc::wait_for(
                proc::wait_by::child_pid, ec, child, &results.back());
        }
        cDebug("------------ Returning to parent ------------");
        cDebug("Child results:");
        for(auto [i, child] : stl_types::enumerate(children))
        {
            auto const& version   = versions[i];
            auto        indicator = results[i] != 0 ? "<====== !!! ====" : "";
            cDebug(
                " - {}: exit={}, GL {} {}.{} {}",
                child,
                results[i],
                magic_enum::enum_name(version.profile),
                version.major,
                version.minor,
                indicator);
        }
        // Close the profile.json properly
        if constexpr(compile_info::profiler::enabled)
            Coffee::State::SwapState("jsonProfiler", {});
        std::quick_exit(0);
    }
#endif
}

void insert_dummy_plug(
    AppLoadableService::entity_container& container,
    dummy_plug::Config&                   dummy_plug)
{
    Coffee::DProfContext _;

    auto const& config = dummy_plug.config;

    auto* input_bus =
        container.service<comp_app::BasicEventBus<Coffee::Input::CIEvent>>();
    auto* window_bus =
        container.service<comp_app::BasicEventBus<Coffee::Display::Event>>();
    auto*      app_bus = container.service<comp_app::BasicEventBus<AppEvent>>();
    const auto type_to_enum = [](nlohmann::json const&   event,
                                 std::string_view const& key) {
        return magic_enum::enum_cast<type_t>(
                   event.value("type", std::string_view()))
            .value();
    };

    if(config.contains("events"))
        for(auto const& event : config["events"])
        {
            if(!event.contains("type"))
                continue;
            const auto type = type_to_enum(event, "type");

            cDebug("- {}: {}", magic_enum::enum_name(type), event.dump());

            switch(type)
            {
            case type_t::controller_axis:
            case type_t::controller_button:
            case type_t::controller_connect:
            case type_t::key:
            case type_t::mouse_button:
            case type_t::mouse_move:
                queue_input_event(input_bus, type, event);
                break;
            case type_t::screenshot:
                // ...
                break;
            case type_t::none:
                break;
            }
        }

    if(config.contains("audio"))
    {
        auto  audio_config  = config["audio"];
        auto& output_config = dummy_plug.audio_config;

        output_config.frequency = audio_config.value("frequency", 44100u);
        output_config.channels  = audio_config.value("channels", 2u);
        output_config.bits      = audio_config.value("bits", 16u);
        output_config.format    = magic_enum::enum_cast<audio_format_t>(
                                   audio_config.value("format", std::string()))
                                   .value_or(audio_format_t::pcm);
    }

    auto end_time = std::chrono::milliseconds(config.value("end_time", 0u));

    rq::runtime_queue::QueueShot(
        rq::runtime_queue::GetCurrentQueue().value(),
        end_time,
        [&container]() {
            auto window = container.service<Windowing>();
            window->close();
        })
        .assume_value();
}

} // namespace comp_app::dummy_plug
