#include "peripherals/libc/types.h"
#include "platforms/file.h"
#include "types.h"
#include <coffee/comp_app/dummy_plug.h>

#include <coffee/comp_app/app_events.h>
#include <coffee/comp_app/gl_config.h>
#include <coffee/comp_app/performance_monitor.h>
#include <coffee/core/debug/formatting.h>
#include <coffee/core/files/cfiles.h>
#include <coffee/core/task_queue/task.h>
#include <coffee/core/types/input/keymap.h>
#include <peripherals/stl/enumerate.h>

#include <algorithm>
#include <chrono>
#include <optional>

#define MAGIC_ENUM_RANGE_MIN 0
#define MAGIC_ENUM_RANGE_MAX 0xFFF
#include <peripherals/stl/magic_enum.hpp>

#if defined(FEATURE_ENABLE_OSMesaComponent)
#include <peripherals/posix/process.h>
#endif

#if defined(__linux__)
#include <climits>
#include <coffee/core/argument_handling.h>
#include <cstdlib>
#include <fcntl.h>
#include <peripherals/posix/process.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

#if defined(COFFEE_EMSCRIPTEN)
#include <coffee/comp_app/bundle.h>
#include <emscripten.h>
#endif

using Coffee::cDebug;
using Coffee::cWarning;
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

    custom,
};

#if defined(__linux__)
/* File-scope (not anonymous-namespace-local) so insert_dummy_plug's
 * emit_events, further down in this file and outside the anonymous
 * namespace below, can see which children this process spawned -- used
 * to lock-step startup (wait for each child's "ready" marker before
 * this process schedules its own scripted events). */
struct spawned_child_t
{
    pid_t       pid;
    std::string id;
};

std::vector<spawned_child_t> spawned_children;
#endif

namespace {

/* Polled controller state, mirroring what a platform layer (SDL2) would
 * hold. Registered as the comp_app::ControllerInput service before the
 * windowing services load, so it takes the service slot and consumers
 * polling ControllerInput::state() see synthetic state. Injecting
 * synthetic *events* alone doesn't work: per-frame movement samples the
 * polled state, never the event stream. */
struct DummyControllerInput
    : comp_app::interfaces::ControllerInput
    , comp_app::AppService<DummyControllerInput, comp_app::ControllerInput>
{
    using type = DummyControllerInput;

    virtual libc_types::u32 count() const final
    {
        return static_cast<libc_types::u32>(m_state.size());
    }

    virtual controller_map state(libc_types::u32 idx) const final
    {
        return idx < m_state.size() ? m_state[idx] : controller_map{};
    }

    virtual comp_app::text_type_t name(libc_types::u32 idx) const final
    {
        return "Dummy Controller " + std::to_string(idx);
    }

    controller_map& ensure(libc_types::u32 idx)
    {
        if(m_state.size() <= idx)
            m_state.resize(idx + 1);
        return m_state[idx];
    }

    std::vector<controller_map> m_state;
};

void queue_input_event(
    comp_app::BasicEventBus<Coffee::Input::CIEvent>* bus,
    DummyControllerInput*                            controllers,
    type_t                                           type,
    nlohmann::json const&                            event)
{
    using Coffee::Input::CIControllerAtomicEvent;
    using Coffee::Input::CIControllerConnectEvent;
    using Coffee::Input::CIKeyEvent;
    using Coffee::Input::CIMouseButtonEvent;
    using Coffee::Input::CIMouseMoveEvent;
    using libc_types::i16;
    using libc_types::u8;

    CIEvent    ievent;
    const auto start_time = std::chrono::milliseconds(event.value("time", 0u));
    /* QueueImmediate rather than QueueShot: both honor the scheduled time,
     * but QueueShot drops tasks whose deadline has already passed — a slow
     * frame (or "time": 0) would silently swallow synthetic input */
    const auto emit_future_event =
        [bus]<typename T>(compo::duration delay, CIEvent event, T data) {
            rq::runtime_queue::QueueImmediate(
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
    /* State mutations ride the same queue as the synthetic events so both
     * views (polled state, event stream) stay in sync frame-wise */
    const auto mutate_future = [](compo::duration delay, auto&& fn) {
        rq::runtime_queue::QueueImmediate(
            rq::runtime_queue::GetCurrentQueue().value(),
            delay,
            std::forward<decltype(fn)>(fn))
            .value();
    };

    using namespace Coffee::Input;
    switch(type)
    {
    case type_t::controller_axis: {
        const auto axis = magic_enum::enum_cast<CIControllerAxisMapping>(
                              "CK_AXIS_" + event.value("axis", std::string()))
                              .value_or(CK_AXIS_BASE);
        const auto controller = event.value("index", 0u);
        const auto axis_idx   = static_cast<u8>(axis - CK_AXIS_BASE);
        const auto value      = static_cast<i16>(
            std::clamp(event.value("value", 0.f), -1.f, 1.f) * 32767.f);

        ievent.type                    = CIEvent::Controller;
        CIControllerAtomicEvent atomic = {
            .value        = value,
            .index        = axis_idx,
            .controller   = static_cast<u8>(controller),
            .button_state = false,
            .axis         = true,
        };
        emit_future_event(start_time, ievent, atomic);
        if(controllers)
            mutate_future(
                start_time, [controllers, controller, axis_idx, value] {
                    controllers->ensure(controller).axes.d[axis_idx] = value;
                });

        /* Optional duration: stick springs back to neutral afterwards */
        if(event.contains("duration"))
        {
            const auto release_time =
                start_time +
                std::chrono::milliseconds(event.value("duration", 0u));
            atomic.value = 0;
            emit_future_event(release_time, ievent, atomic);
            if(controllers)
                mutate_future(
                    release_time, [controllers, controller, axis_idx] {
                        controllers->ensure(controller).axes.d[axis_idx] = 0;
                    });
        }
        break;
    }
    case type_t::controller_button: {
        const auto button =
            magic_enum::enum_cast<CIControllerButtonMapping>(
                "CK_BUTTON_" + event.value("button", std::string()))
                .value_or(CK_BUTTON_BASE);
        const auto controller = event.value("index", 0u);
        const auto bit        = static_cast<u8>(button - CK_BUTTON_BASE);
        const auto release_time =
            start_time + std::chrono::milliseconds(event.value("duration", 0u));

        /* CIControllerAtomicEvent is BaseEvent<CIEvent::Controller>;
         * this previously went out as ControllerUpdate, which readers
         * decode as a connect/disconnect payload */
        ievent.type                           = CIEvent::Controller;
        CIControllerAtomicEvent controller_ev = {
            .index        = bit,
            .controller   = static_cast<u8>(controller),
            .button_state = true,
            .axis         = false,
        };
        emit_future_event(start_time, ievent, controller_ev);
        controller_ev.button_state = false;
        emit_future_event(release_time, ievent, controller_ev);
        if(controllers)
        {
            mutate_future(start_time, [controllers, controller, bit] {
                controllers->ensure(controller).buttons.d |= (1u << bit);
            });
            mutate_future(release_time, [controllers, controller, bit] {
                controllers->ensure(controller).buttons.d &=
                    ~static_cast<libc_types::u16>(1u << bit);
            });
        }
        break;
    }
    case type_t::controller_connect: {
        const auto controller = event.value("index", 0u);
        /* Seat assignment listens for ControllerConnect (what the SDL2
         * layer emits), not the atomic update event */
        ievent.type                      = CIEvent::ControllerConnect;
        CIControllerConnectEvent connect = {
            .index        = static_cast<libc_types::u16>(controller),
            .player_index = static_cast<i16>(controller),
            .connected    = true,
        };
        emit_future_event(start_time, ievent, connect);
        if(controllers)
            mutate_future(start_time, [controllers, controller] {
                controllers->ensure(controller);
            });
        break;
    }
    case type_t::key: {
        using Coffee::Input::Latin1Mapping;

        ievent.type = CIEvent::Keyboard;
        CIKeyEvent key;
        const auto key_value = event.value("key", std::string());
        key.key =
            magic_enum::enum_cast<Latin1Mapping>(key_value).value_or(CK_Null);
        if(key.key == CK_Null)
            key.key =
                magic_enum::enum_cast<CISpecialKeyMapping>(key_value).value_or(
                    static_cast<CISpecialKeyMapping>(0));
        key.mod = CIKeyEvent::PressedModifier;
        emit_future_event(start_time, ievent, key);
        auto unpress_delay =
            std::chrono::milliseconds(event.value("duration", 0u));
        key.mod = CIKeyEvent::NoneModifier;
        emit_future_event(start_time + unpress_delay, ievent, key);
        break;
    }
    case type_t::mouse_move: {
        ievent.type = CIEvent::MouseMove;
        CIMouseMoveEvent move;
        move.origin = {event.value("x", 0.f), event.value("y", 0.f)};
        move.delta  = {};
        emit_future_event(start_time, ievent, move);
        break;
    }
    default:
        break;
    }
}

#if defined(__linux__)
/* Multi-process test orchestration: a "spawn" array in the dummy plug
 * config forks+execs this same binary N times, each child seeded with its
 * own dummy plug config (and optionally extra argv, e.g. --server) — so a
 * client/server scenario is one `cb run` with no wrapper script managing
 * processes. fork+exec (not bare fork): by this point the process may
 * already have threads and initialized library state; exec gives each
 * child a clean slate, and the child's own config drives it from there.
 *
 * Entry schema: {"config": "path.json"  (required),
 *                "id":     "client0"    (child TMPDIR name; default spawnN),
 *                "args":   ["--server", "..."]  (appended to argv)}
 * Top-level "spawn_grace_ms" (default 30000): how long the parent waits at
 * exit for children before SIGKILLing them.
 *
 * The parent reaps children at exit and turns any child failure (nonzero
 * exit, signal death, or having to be killed) into its own nonzero exit
 * code, so CI needs no extra bookkeeping. */
libc_types::u32 spawn_grace_ms = 30000;

void reap_spawned_children()
{
    using namespace std::chrono;
    /* Logging infrastructure may be torn down during exit — stderr only */
    auto deadline = steady_clock::now() + milliseconds(spawn_grace_ms);
    bool failed   = false;
    auto pending  = spawned_children;
    while(!pending.empty() && steady_clock::now() < deadline)
    {
        for(auto it = pending.begin(); it != pending.end();)
        {
            int   status = 0;
            pid_t r      = ::waitpid(it->pid, &status, WNOHANG);
            if(r == it->pid)
            {
                int code = WIFEXITED(status)     ? WEXITSTATUS(status)
                           : WIFSIGNALED(status) ? 128 + WTERMSIG(status)
                                                 : -1;
                fprintf(
                    stderr,
                    "dummy plug spawn: child %s (pid %d) exited with %d\n",
                    it->id.c_str(),
                    it->pid,
                    code);
                failed = failed || code != 0;
                it     = pending.erase(it);
            } else if(r < 0)
            {
                /* Already reaped elsewhere or gone */
                it = pending.erase(it);
            } else
                ++it;
        }
        if(!pending.empty())
            ::usleep(200 * 1000);
    }
    for(auto const& child : pending)
    {
        fprintf(
            stderr,
            "dummy plug spawn: child %s (pid %d) still running after grace "
            "period, killing\n",
            child.id.c_str(),
            child.pid);
        ::kill(child.pid, SIGKILL);
        ::waitpid(child.pid, nullptr, 0);
        failed = true;
    }
    if(failed)
        ::_exit(1);
}

void spawn_dummy_plug_children(nlohmann::json const& config)
{
    using platform::url::constructors::MkUrl;
    using semantic::RSCA;

    /* Children never spawn their own children: a child config that (by
     * mistake or reuse) also contains "spawn" would otherwise fork-bomb */
    if(platform::env::var("COFFEE_DUMMY_PLUG_SPAWNED").has_value())
    {
        cDebug("dummy plug spawn: already a spawned child, ignoring \"spawn\"");
        return;
    }

    spawn_grace_ms = config.value("spawn_grace_ms", 30000u);

    auto const& args_in = Coffee::GetInitArgs();

    /* When this process was started through an explicit dynamic loader
     * (`ld-linux.so [--library-path ...] <binary> <args>` — how CI runs
     * downloaded artifacts against a sysroot), /proc/self/exe is the
     * LOADER, not this program. Re-exec'ing it then needs the program
     * path as its first real argument: the loader ignores argv[0] and
     * loads argv[1] — without this shift it tried to load our first CLI
     * argument (the assets directory) as an ELF. Detected by comparing
     * /proc/self/exe against realpath(argv[0]); the loader picks up
     * library paths from the inherited LD_LIBRARY_PATH. */
    bool via_loader = false;
    {
        char    self_buf[PATH_MAX];
        ssize_t n =
            ::readlink("/proc/self/exe", self_buf, sizeof(self_buf) - 1);
        if(n > 0)
        {
            self_buf[n] = '\0';
            char argv0_buf[PATH_MAX];
            if(!args_in.empty() && args_in[0] &&
               ::realpath(args_in[0], argv0_buf))
                via_loader = std::string_view(self_buf) != argv0_buf;
            else
                via_loader = std::string_view(self_buf).find("ld-") !=
                             std::string_view::npos;
        }
    }

    libc_types::u32 index = 0;
    for(auto const& entry : config["spawn"])
    {
        const auto cfg_path = entry.value("config", std::string{});
        if(cfg_path.empty())
        {
            Coffee::Logging::cFatal(
                "dummy plug spawn: entry without \"config\", skipping");
            continue;
        }
        const auto id = entry.value("id", fmt::format("spawn{}", index));
        ++index;

        /* Child gets its own TMPDIR under the parent's temp dir so
         * state.json/screenshots/logs don't collide */
        const auto child_tmp = *MkUrl(id, RSCA::TempFile);
        ::mkdir(child_tmp.c_str(), 0755);
        const auto child_log = child_tmp + "/log.txt";

        std::vector<std::string> args;
        if(via_loader)
            /* argv[0] for the loader itself (cosmetic); the program
             * (args_in[0]) then lands in argv[1] where the loader
             * expects it */
            args.push_back("dynamic-loader");
        for(auto const* arg : args_in)
            if(arg)
                args.push_back(arg);
        if(entry.contains("args"))
            for(auto const& extra : entry["args"])
                args.push_back(extra.get<std::string>());

        pid_t child = ::fork();
        if(child < 0)
        {
            Coffee::Logging::cFatal("dummy plug spawn: fork failed");
            continue;
        }
        if(child == 0)
        {
            /* Child: die with the parent, redirect output to the child's
             * log file, point the dummy plug at the child config, exec a
             * fresh copy of ourselves */
            ::prctl(PR_SET_PDEATHSIG, SIGKILL);
            int log_fd =
                ::open(child_log.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if(log_fd >= 0)
            {
                ::dup2(log_fd, STDOUT_FILENO);
                ::dup2(log_fd, STDERR_FILENO);
                ::close(log_fd);
            }
            ::setenv("DUMMY_PLUG_CONFIG", cfg_path.c_str(), 1);
            ::setenv("TMPDIR", child_tmp.c_str(), 1);
            ::setenv("COFFEE_DUMMY_PLUG_SPAWNED", "1", 1);

            std::vector<const char*> argv;
            for(auto const& arg : args)
                argv.push_back(arg.c_str());
            argv.push_back(nullptr);
            ::execv("/proc/self/exe", const_cast<char* const*>(argv.data()));
            /* exec failed */
            ::_exit(127);
        }

        cDebug(
            "dummy plug spawn: child {} (pid {}) config={} tmp={}",
            id,
            child,
            cfg_path,
            child_tmp);
        if(spawned_children.empty())
            std::atexit(reap_spawned_children);
        spawned_children.push_back({child, id});
    }
}
#endif

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

    cDebug(
        "Dummy plug plan: graphics={} audio={} events={}",
        config.contains("graphics"),
        config.contains("audio"),
        [&] -> libc_types::u32 {
            if(!config.contains("events"))
                return 0u;
            if(!config["events"].is_array())
                return 0u;
            return config["events"].size();
        }());

#if defined(__linux__)
    if(config.contains("spawn") && config["spawn"].is_array())
        spawn_dummy_plug_children(config);
#endif

    if(config.contains("frame_delta"))
    {
        dummy_plug.frame_delta = std::chrono::microseconds(
            static_cast<libc_types::u32>(
                config.value("frame_delta", 0.0) * 1000));
        /* Replace the original timestamp provider, so we can step the frames
         * consistently. As long as everything is based on ECS-provided
         * time_points, it should be deterministic. */
        container.set_timestamp_provider(
            [epoch = compo::clock::time_point() + container.epoch_offset(),
             &dummy_plug]() {
                return epoch + dummy_plug.frame_delta * dummy_plug.frame_index;
            });
        container.add_frame_end_callback(
            [&dummy_plug]() { dummy_plug.frame_index++; });
        rq::runtime_queue::OverrideClock(
            [epoch = rq::detail::time_point() + container.epoch_offset(),
             &dummy_plug]() {
                return epoch + dummy_plug.frame_delta * dummy_plug.frame_index;
            });
    }

    if(!config.contains("graphics"))
        return;

    dummy_plug.swrender = config["graphics"].value("software_renderer", "none");
    dummy_plug.screenshot_quality =
        config["graphics"].value("screenshot_quality", 30);

    cDebug("Rendering backend selected: {}", dummy_plug.swrender);
    auto& glConfig = container.service<AppLoader>()->config<GLConfig>();
    if(dummy_plug.swrender == "mesa")
    {
        cDebug("MESA software rendering activated");
        glConfig.profile       = GLConfig::Core;
        glConfig.version.major = 4;
        glConfig.version.minor = 5;

        dummy_plug.graphics_config            = nlohmann::json();
        dummy_plug.graphics_config["profile"] = "Core";
        dummy_plug.graphics_config["major"]   = 4u;
        dummy_plug.graphics_config["minor"]   = 5u;
    } else if(dummy_plug.swrender == "angle")
    {
        cDebug("ANGLE software rendering activated");
        glConfig.profile       = GLConfig::Embedded;
        glConfig.version.major = 3;
        glConfig.version.minor = 2;

        dummy_plug.graphics_config            = nlohmann::json();
        dummy_plug.graphics_config["profile"] = "Embedded";
        dummy_plug.graphics_config["major"]   = 3u;
        dummy_plug.graphics_config["minor"]   = 2u;
    } else if(
        dummy_plug.swrender == "llvmpipe" ||
        dummy_plug.swrender == "surfaceless")
    {
        /* Both select the EGL surfaceless backend. "surfaceless" runs on
         * whatever device Mesa picks (the hardware GPU by default); "llvmpipe"
         * is the same path kept for the software-named config. */
        if(dummy_plug.swrender == "surfaceless")
            cDebug("EGL surfaceless (hardware) rendering activated");
        else
            cDebug("llvmpipe software rendering activated");
        glConfig.profile       = GLConfig::Core;
        glConfig.version.major = 4u;
        glConfig.version.minor = 6u;

        dummy_plug.graphics_config = nlohmann::json();
        dummy_plug.graphics_config["profile"] =
            magic_enum::enum_name(glConfig.profile);
        dummy_plug.graphics_config["major"] = glConfig.version.major;
        dummy_plug.graphics_config["minor"] = glConfig.version.minor;
    } else
    {
        dummy_plug.graphics_config = nlohmann::json();
        dummy_plug.graphics_config["profile"] =
            magic_enum::enum_name(glConfig.profile);
        dummy_plug.graphics_config["major"] = glConfig.version.major;
        dummy_plug.graphics_config["minor"] = glConfig.version.minor;
    }

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

    const auto apply_version = [&glConfig,
                                &dummy_plug](version_t const& version) {
        if(dummy_plug.swrender == "mesa")
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
            glConfig.depthFmt =
                version.depth == 32 ? pix_fmt::Depth32 : pix_fmt::Depth16;
        } else if(
            dummy_plug.swrender == "llvmpipe" ||
            dummy_plug.swrender == "surfaceless")
        {
            glConfig.profile       = version.profile;
            glConfig.version.major = version.major;
            glConfig.version.minor = version.minor;
            glConfig.depthFmt =
                version.depth == 32 ? pix_fmt::Depth32F : pix_fmt::Depth16F;
            glConfig.depthFmt = pix_fmt::Depth24Stencil8;
        }

        dummy_plug.graphics_config = nlohmann::json();
        dummy_plug.graphics_config["profile"] =
            magic_enum::enum_name(version.profile);
        dummy_plug.graphics_config["major"] = version.major;
        dummy_plug.graphics_config["minor"] = version.minor;
    };

    if(versions.empty())
        return;

    /* Special case: If there's only one config, don't fork */
    if(versions.size() == 1 || dummy_plug.swrender == "none")
    {
        apply_version(versions.at(0));
        return;
    }

#if defined(FEATURE_ENABLE_OSMesaComponent)
    using platform::url::constructors::MkUrl;
    using semantic::RSCA;

    using namespace platform::common::posix;

    // We're gonna do some funny business here; we'll fork the process into X
    // amount of processes, each with their own GL config, all running in
    // software mode. It's gonna be great!
    std::vector<std::tuple<pid_t, version_t, int>> children;
    for(auto const& version : versions)
    {
        cDebug("-------------------------------------------------");
        cDebug("------------ Spawning child ---------------------");
        pid_t    child_pid{};
        posix_ec ec;
        if(auto res = proc::fork(child_pid, ec);
           res == proc::fork_process::error)
        {
            Coffee::Logging::cFatal("Failed to fork: {}", ec.error_message);
            C_BREAK();
        } else if(res == proc::fork_process::child)
        {
            apply_version(version);

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
        {
            int result{};
            proc::wait_for(proc::wait_by::child_pid, ec, child_pid, &result);
            cDebug("------------ Returning to parent ------------");
            children.push_back(std::make_tuple(child_pid, version, result));
        }
    }

    if(!children.empty())
    {
        cDebug("-------------------------------------------------");
        cDebug("Child results:");
        for(auto [child, version, result] : children)
        {
            auto indicator = result != 0 ? " <====== !!! ====" : "";
            cDebug(
                " - {}: exit={}, GL {} {}.{}{}",
                child,
                result,
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

namespace {
struct FrameCounter : compo::SubsystemBase
{
    using type = FrameCounter;

    virtual void start_frame(ContainerProxy&, time_point const&) final
    {
        frame_counter++;
    }

    libc_types::u64 frame_counter{0};
};
} // namespace

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
    auto* app_bus = container.service<comp_app::BasicEventBus<AppEvent>>();

    /* Take the ControllerInput service slot before the windowing layer
     * registers its own (AppLoader's service_register skips occupied
     * slots), so controller state polled by the app is synthetic. */
    auto& dummy_controllers =
        DummyControllerInput::register_service<DummyControllerInput>(container);

    if(config.contains("events"))
    {
        auto emit_events = [&dummy_plug,
                            &config,
                            input_bus,
                            &dummy_controllers,
                            &container,
                            flag         = false,
                            ready_marked = false,
                            children_wait_start =
                                std::optional<std::chrono::steady_clock::
                                                  time_point>{}]() mutable {
            using namespace Coffee::resource_literals;
            auto* app_info = container.service<comp_app::AppInfo>();
            if(app_info->state() != comp_app::interfaces::AppInfo::loaded)
            {
                cDebug("Waiting for application to load");
                return;
            }

            /* Lock-step startup: mark this process ready (for any parent
             * that spawned it to see), then wait for our own spawned
             * children (if any) to do the same before scheduling our own
             * scripted events. Without this, a parent whose own load is
             * fast can run its whole scenario and exit while a child
             * still stuck loading (e.g. sound decode under CI's CPU
             * contention) never gets far enough to fire anything. */
            if(!ready_marked)
            {
                auto marker = Coffee::Resource("ready"_tmpfile);
                marker = semantic::BytesConst::ofString(std::string_view("1"));
                Coffee::FileCommit(
                    marker,
                    semantic::RSCA::NewFile | semantic::RSCA::Discard |
                        semantic::RSCA::WriteOnly);
                ready_marked = true;
            }
#if defined(__linux__)
            if(!spawned_children.empty())
            {
                using namespace std::chrono;
                if(!children_wait_start)
                    children_wait_start = steady_clock::now();
                auto elapsed = steady_clock::now() - *children_wait_start;
                auto timeout = milliseconds(
                    config.value("child_ready_timeout_ms", 300000u));

                for(auto const& child : spawned_children)
                {
                    if(Coffee::FileExists(
                           Coffee::Resource(
                               child.id + "/ready", semantic::RSCA::TempFile)))
                        continue;

                    if(elapsed < timeout)
                    {
                        cDebug(
                            "Waiting for spawned child \"{}\" to finish "
                            "loading",
                            child.id);
                        return;
                    }
                    /* Give up waiting on this child rather than hang
                     * forever: end_time (below) is only ever scheduled
                     * once this function gets past this gate, so with no
                     * timeout a child that never becomes ready -- stuck,
                     * crashed without writing its marker, or just too
                     * slow -- left the parent with no self-termination
                     * path at all. Found via an actual CI run: server
                     * healthy at 15 FPS, stuck on this wait until GitHub's
                     * own 6-hour job cap killed it. */
                    cWarning(
                        "Spawned child \"{}\" did not finish loading "
                        "within {}ms -- proceeding without it",
                        child.id,
                        timeout.count());
                }
            }
#endif

            if(flag)
                return;
            cDebug("Queueing dummy plug events");

            const auto type_to_enum = [](nlohmann::json const&   event,
                                         std::string_view const& key) {
                return magic_enum::enum_cast<type_t>(
                           event.value("type", std::string_view()))
                    .value_or(type_t::custom);
            };
            auto& perf_monitor =
                container.subsystem_cast<comp_app::PerformanceMonitor>();
            perf_monitor.m_screenshot_quality = dummy_plug.screenshot_quality;
            perf_monitor.m_synchronous_screenshots = true;
            auto& dummy_bus = container.subsystem_cast<DummyEventBus>();
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
                    queue_input_event(
                        input_bus, &dummy_controllers, type, event);
                    break;
                case type_t::screenshot: {
                    auto start_time =
                        std::chrono::milliseconds(event.value("time", 0u));
                    rq::runtime_queue::QueueShot(
                        rq::runtime_queue::GetCurrentQueue().value(),
                        start_time,
                        [&perf_monitor, &container, event]() {
                            PerformanceMonitor::proxy_type proxy(container);
                            cDebug("Capturing dummyplug screenshot");
                            perf_monitor.capture_screenshot(
                                proxy,
                                event.value("name", "dummy_screenshot"),
                                container.relative_timestamp());
                        })
                        .assume_value();
                    break;
                }
                case type_t::custom: {
                    DummyEvent out{};
                    out.event = event.value("type", std::string{});
                    out.data  = event;
                    out.data.erase("type");
                    out.data.erase("time");
                    auto start_time =
                        std::chrono::milliseconds(event.value("time", 0u));
                    rq::runtime_queue::QueueShot(
                        rq::runtime_queue::GetCurrentQueue().value(),
                        start_time,
                        [&dummy_bus, out]() mutable {
                            cDebug(
                                "Injecting custom dummy event: {} => {}",
                                out.event,
                                out.data.dump(2));
                            dummy_bus.process(out, nullptr);
                        })
                        .assume_value();
                    break;
                }
                case type_t::none:
                    break;
                }
            }

            auto end_time =
                std::chrono::milliseconds(config.value("end_time", 0u));

            if(config.contains("end_time"))
            {
                auto& counter =
                    container.register_subsystem_inplace<FrameCounter>();
                rq::runtime_queue::QueueShot(
                    rq::runtime_queue::GetCurrentQueue().value(),
                    end_time,
                    [&container, &counter]() {
                        auto  window  = container.service<Windowing>();
                        auto* appInfo = container.service<comp_app::AppInfo>();
                        appInfo->add(
                            "run:totalFrames",
                            std::to_string(counter.frame_counter));
                        appInfo->add(
                            "run:dummyPlug",
                            platform::url::constructors::MkUrl(
                                platform::env::var("DUMMY_PLUG_CONFIG").value())
                                .path()
                                .fileBasename()
                                .internUrl);
                        window->close();
                    })
                    .assume_value();
            }
            flag = true;
        };

        rq::runtime_queue::QueuePeriodic(
            rq::runtime_queue::GetCurrentQueue().value(),
            std::chrono::milliseconds(100),
            std::move(emit_events))
            .assume_value();
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
}

#if defined(COFFEE_EMSCRIPTEN)
extern "C" EMSCRIPTEN_KEEPALIVE int coffee_app_loaded()
{
    auto& container = ::comp_app::createContainer();
    auto* app_info  = container.service<comp_app::AppInfo>();
    return (app_info &&
            app_info->state() == comp_app::interfaces::AppInfo::loaded)
               ? 1
               : 0;
}

extern "C" EMSCRIPTEN_KEEPALIVE void coffee_dummy_plug_event(
    const char* json_str)
{
    if(!json_str)
        return;

    nlohmann::json event;
    try
    {
        event = nlohmann::json::parse(json_str);
    } catch(std::exception const& e)
    {
        cDebug("dummy plug live: invalid event JSON: {}", e.what());
        return;
    }

    if(!event.is_object() || !event.contains("type"))
        return;

    auto&      container = ::comp_app::createContainer();
    const auto type =
        magic_enum::enum_cast<type_t>(event.value("type", std::string_view()))
            .value_or(type_t::custom);
    event["time"] = 0u;

    switch(type)
    {
    case type_t::controller_axis:
    case type_t::controller_button:
    case type_t::controller_connect:
    case type_t::key:
    case type_t::mouse_button:
    case type_t::mouse_move: {
        if(auto* input_bus =
               container
                   .service<comp_app::BasicEventBus<Coffee::Input::CIEvent>>())
            queue_input_event(
                input_bus,
                C_DCAST<DummyControllerInput>(
                    container.service<comp_app::ControllerInput>()),
                type,
                event);
        break;
    }
    case type_t::custom: {
        DummyEvent out{};
        out.event = event.value("type", std::string{});
        out.data  = event;
        out.data.erase("type");
        out.data.erase("time");
        // cDebug("dummy plug live: custom event {} => {}", out.event,
        //        out.data.dump());
        container.subsystem_cast<DummyEventBus>().process(out, nullptr);
        break;
    }
    case type_t::screenshot:
    case type_t::none:
        break;
    }
}
#endif

} // namespace comp_app::dummy_plug
