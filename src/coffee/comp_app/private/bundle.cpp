#include <coffee/comp_app/bundle.h>

#include <coffee/comp_app/app_events.h>
#include <coffee/comp_app/eventapp_wrapper.h>
#include <coffee/comp_app/file_mapper.h>
#include <coffee/comp_app/file_watcher.h>
#include <coffee/comp_app/gl_config.h>
#include <coffee/comp_app/performance_monitor.h>
#include <coffee/comp_app/stat_providers.h>

#include <coffee/core/CProfiling>
#include <coffee/core/base_state.h>
#include <coffee/core/coffee.h>
#include <coffee/core/task_queue/task.h>
#include <coffee/core/types/display/event.h>
#include <coffee/core/types/input/event_types.h>
#include <coffee/image/cimage.h>

#include <peripherals/libc/signals.h>
#include <peripherals/stl/base64.h>
#include <peripherals/stl/magic_enum.hpp>
#include <peripherals/stl/string_ops.h>

#include <platforms/environment.h>
#include <platforms/stacktrace.h>

#include <coffee/core/debug/formatting.h>
#include <coffee/strings/format.h>

#if defined(COFFEE_EMSCRIPTEN)
#include <emscripten/emscripten.h>
#endif

#if defined(FEATURE_ENABLE_EGLComponent)
#include <coffee/egl/egl_comp.h>
#endif

#if defined(FEATURE_ENABLE_X11Component)
#include <coffee/x11/x11_comp.h>
#endif

#if defined(FEATURE_ENABLE_GLADComponent_Dynamic)      \
    || defined(FEATURE_ENABLE_GLADComponent_ESDynamic) \
    || defined(FEATURE_ENABLE_GLADComponent_ES2Dynamic)
#include <coffee/glad/glad_comp.h>
#endif

#if defined(FEATURE_ENABLE_SDL2Components)
#include <coffee/sdl2_comp/sdl2_components.h>
#endif

#if defined(FEATURE_ENABLE_GLKitComponent)
#include <coffee/glkit/glkit_comp.h>
#endif

#if defined(FEATURE_ENABLE_ANativeComponent)
#include <coffee/anative/anative_comp.h>
#endif

#if defined(FEATURE_ENABLE_CogComponent)
#include <coffee/cog_comp/cog_comp.h>
#endif

#if defined(FEATURE_ENABLE_UIKitGestures)
#include <coffee/uikit/uikit_comp.h>
#endif

#if defined(FEATURE_ENABLE_EmscriptenComponents)
#include <coffee/emscripten_comp/emscripten_components.h>
#endif

#if defined(FEATURE_ENABLE_DispManXComponent)
#include <coffee/dispmanx/dispmanx_comp.h>
#endif

#if defined(FEATURE_ENABLE_GLScreenshot_ES2Dynamic)   \
    || defined(FEATURE_ENABLE_GLScreenshot_ESDynamic) \
    || defined(FEATURE_ENABLE_GLScreenshot_ES)        \
    || defined(FEATURE_ENABLE_GLScreenshot_Dynamic)
#include <glscreenshot/screenshot.h>
#endif

#if defined(FEATURE_ENABLE_GLKitComponent) \
    || defined(FEATURE_ENABLE_ANativeComponent) || defined(COFFEE_EMSCRIPTEN)
#define USES_LINKED_GL 1
#endif

#if defined(FEATURE_ENABLE_GLADComponent_Dynamic)       \
    || defined(FEATURE_ENABLE_GLADComponent_ESDynamic)  \
    || defined(FEATURE_ENABLE_GLADComponent_ES2Dynamic) \
    || defined(FEATURE_ENABLE_EGLComponent)             \
    || defined(FEATURE_ENABLE_SDL2Components) || USES_LINKED_GL
#define USES_GL 1
#endif

using Coffee::Logging::cVerbose;

namespace comp_app {

namespace {

[[maybe_unused]] void printConfig(AppLoader& loader)
{
    using Coffee::cDebug;
#if USES_GL
    {
        auto& glConf = loader.config<GLConfig>();
        cDebug(
            "comp_app::GLConfig: version={0}:{1}.{2} ({3}) fmt={4} depth={5}",
            glConf.profile == GLConfig::Profile::Core ? "core" : "es",
            glConf.version.major,
            glConf.version.minor,
            glConf.versionIsFixed ? "fixed" : "dynamic",
            magic_enum::enum_name(glConf.framebufferFmt),
            magic_enum::enum_name(glConf.depthFmt));
    }
#endif
    {
        using enum_helpers::feval;
        auto& windowConfig = loader.config<WindowConfig>();
        cDebug(
            "comp_app::WindowConfig: size={0}x{1} title={2} windowed={3} "
            "fullscreen={4} highdpi={5}",
            windowConfig.size.w,
            windowConfig.size.h,
            windowConfig.title,
            feval(windowConfig.flags, window_flags_t::windowed),
            feval(windowConfig.flags, window_flags_t::fullscreen),
            feval(windowConfig.flags, window_flags_t::high_dpi));
    }
}

rq::runtime_queue* loop_main_queue = nullptr;
bool               is_loaded       = false;

} // namespace

void setup_container(detail::EntityContainer& container)
{
    if(is_loaded)
        return;

    using namespace Coffee;
    DProfContext _("Bundle::Setup");

#if defined(COFFEE_EMSCRIPTEN)
    emscripten_set_main_loop_expected_blockers(1);
#endif

    app_error appec;
    auto      services = container.services_with<AppLoadableService>(
        compo::sorter::
            priority_ranked<AppLoadableService, compo::SubsystemBase>);
    cDebug("{0} services to load:", services.size());
    for(auto& service : services)
        cDebug(
            " - {0} : {1}",
            platform::stacktrace::demangle::name(
                dynamic_cast<compo::SubsystemBase*>(service)
                    ->subsystem_name()
                    .data()),
            magic_enum::enum_name(service->current_state()));
    for(auto& service : services)
    {
        if(service->is_loaded())
            continue;
        cDebug(
            " - Loading {0}",
            platform::stacktrace::demangle::name(
                dynamic_cast<compo::SubsystemBase*>(service)
                    ->subsystem_name()
                    .data()));
        service->do_load(container, appec);
        C_ERROR_CHECK(appec);
    }

    is_loaded = true;
}

bool loop_container(detail::EntityContainer& container)
{
    auto window = container.service<Windowing>();
    if(!window)
        return true;
    if(window->notifiedClose())
        return false;
    if(!loop_main_queue)
        if(auto queue = rq::runtime_queue::GetCurrentQueue(); queue.has_value())
        {
            using namespace std::chrono_literals;

            loop_main_queue = queue.value();

            C_UNUSED(auto res) = rq::runtime_queue::QueuePeriodic(
                loop_main_queue, 500ms, [&container]() {
                    auto services
                        = container.services_with<AppLoadableService>();
                    for(auto& loadable : services)
                    {
                        if(!loadable->is_loaded())
                        {
                            app_error ec;
                            loadable->do_load(container, ec);
                        }
                    }
                });
        }

    container.exec();
    if(loop_main_queue)
        loop_main_queue->execute_tasks();
    return true;
}

void cleanup_container(detail::EntityContainer& container)
{
    using namespace Coffee;
    DProfContext _("Bundle::Cleanup");

    app_error appec;
    auto      services = container.services_with<AppLoadableService>(
        Components::reverse_query);
    for(auto& service : services)
    {
        if(service->is_loaded())
            continue;
        service->do_unload(container, appec);
    }

    loop_main_queue = nullptr;
    is_loaded       = false;
}

void setup_and_loop_container(detail::EntityContainer& container)
{
    if(!is_loaded)
        setup_container(container);

    loop_container(container);
}

struct DefaultAppInfo : interfaces::AppInfo, AppService<DefaultAppInfo, AppInfo>
{
    virtual void        add(text_type key, text_type value) final;
    virtual text_type_t get(text_type key) final;
};

void DefaultAppInfo::add(text_type key, text_type value)
{
    Coffee::ExtraData::Add(key, value);
}

text_type_t DefaultAppInfo::get(text_type key)
{
    auto data = Coffee::ExtraData::Get();
    auto it   = data.find(key);

    if(it == data.end())
        return {};

    return it->second;
}

void emscripten_loop()
{
    setup_and_loop_container(createContainer());
}

struct app_loadable_matcher
{
    static bool match(Coffee::Components::SubsystemBase* subsys)
    {
        return C_DCAST<AppLoadableService>(subsys) != nullptr;
    }
};

detail::EntityContainer& createContainer()
{
    static std::shared_ptr<detail::EntityContainer> container;

    if(container)
        return *container;

    container = std::make_shared<detail::EntityContainer>();

    if constexpr(
        compile_info::platform::is_unix
        && !compile_info::platform::is_emscripten)
    {
        auto handler = [](int) {
            if(!container)
                return;
            if(auto windowing = container->service<Windowing>())
                windowing->close();
        };
        libc::signal::install(libc::signal::sig::interrupt, handler);
        libc::signal::install(libc::signal::sig::terminate, handler);
    }

    return *container;
}

void configureDefaults(AppLoader& loader)
{
    Coffee::DProfContext _("comp_app::configureDefaults");

    rq::runtime_queue::CreateNewQueue("Main").assume_value();

    loader.addConfigs<detail::TypeList<
        WindowConfig,
        ControllerConfig,
        TouchConfig,
        GraphicsBindingConfig>>();

#if USES_GL
    loader.addConfigs<detail::TypeList<GLConfig>>();

    auto& glConfig          = loader.config<GLConfig>();
    glConfig.framebufferFmt = PixFmt::RGBA8;
    glConfig.depthFmt       = PixFmt::Depth24Stencil8;

#if defined(FEATURE_ENABLE_GLeamCommon_ES2) \
    || defined(FEATURE_ENABLE_GLeamCommon_ES2Dynamic)
    glConfig.framebufferFmt = PixFmt::RGB565;
    glConfig.depthFmt       = PixFmt::Depth24Stencil8;
    glConfig.profile        = GLConfig::Embedded;
    glConfig.version.major  = 2;
    glConfig.version.minor  = 0;
#elif defined(FEATURE_ENABLE_GLeamCommon_ES) \
    || defined(FEATURE_ENABLE_GLeamCommon_ESDynamic)
    glConfig.framebufferFmt = PixFmt::RGB565;
    glConfig.profile        = GLConfig::Embedded;
    glConfig.version.major  = 3;
    glConfig.version.minor  = 2;
#else
    glConfig.profile       = GLConfig::Core;
    glConfig.version.major = 4;
    glConfig.version.minor = 6;
#endif
#endif

    /*
     * We *can* enable sRGB on desktop, and on Android
     * On iOS we don't have as much control over the colorspace used
     */
    if(compile_info::platform::is_android
       || (compile_info::platform::is_linux && !compile_info::platform::is_iot)
       || compile_info::platform::is_macos
       || compile_info::platform::is_windows)
        glConfig.framebufferFmt = PixFmt::SRGB8;

#if defined(FEATURE_ENABLE_X11Component)
    loader.addConfigs<detail::TypeList<x11::X11Config>>();
#if defined(COFFEE_MAEMO)
    auto& x11Config  = loader.config<x11::X11Config>();
    x11Config.hildon = true;
#endif
#endif

    auto& window = loader.config<WindowConfig>();

    window.title = Coffee::State::GetAppData()->application_name;
    window.size  = {1280, 720};
    if(compile_info::platform::is_macos || compile_info::platform::is_ios
       || compile_info::platform::is_android
       || compile_info::platform::is_windows)
        window.flags |= window_flags_t::high_dpi;
}

void addDefaults(
    AppLoadableService::entity_container& container,
    AppLoader&                            loader,
    app_error&                            ec)
{
    Coffee::ProfContext _("comp_app::addDefaults");

    if constexpr(compile_info::debug_mode)
    {
        if(auto loader = container.service<AppLoader>())
            printConfig(*loader);
    }

    cVerbose(10, "Loading event bus: InputEvent, DisplayEvent, AppEvent");
    loader.loadAll<subsystem_list<
        BasicEventBus<Coffee::Input::CIEvent>,
        BasicEventBus<Coffee::Display::Event>,
        BasicEventBus<AppEvent>,
        DefaultAppInfo>>(container, ec);
    container.register_subsystem_inplace<FileMapper>();
    container.register_subsystem_inplace<FileWatcher>();

#if defined(FEATURE_ENABLE_ANativeComponent)
    loader.loadAll<subsystem_list<anative::AndroidEventBus>>(container, ec);
#endif

    BasicEventBus<AppEvent>* app_bus
        = container.service<BasicEventBus<AppEvent>>();
    app_bus->addEventFunction<LifecycleEvent>(
        2048, [&container](AppEvent&, LifecycleEvent* event) {
            switch(event->lifecycle_type)
            {
                //        case LifecycleEvent::Startup:
                //            break;
            case LifecycleEvent::Foreground:
                setup_container(container);
                break;
            case LifecycleEvent::FrameRequested:
                loop_container(container);
                break;
#if defined(COFFEE_APPLE_MOBILE)
            case LifecycleEvent::Background:
#endif
            case LifecycleEvent::ResourcesLost:
            case LifecycleEvent::Terminate:
                cleanup_container(container);
                break;
            default:
                break;
            }
        });

    auto& appInfo = *container.service<AppInfo>();

    if constexpr(compile_info::debug_mode && compile_info::profiler::enabled)
    {
        loader.registerAll<type_safety::type_list_t<PerformanceMonitor>>(
            container, ec);
        C_ERROR_CHECK(ec);
    }

    /* Selection of window/event manager */
    cVerbose(10, "Loading windowing library");
#if defined(FEATURE_ENABLE_SDL2Components)
    loader.registerAll<sdl2::Services>(container, ec);
    C_ERROR_CHECK(ec);

#if !defined(FEATURE_ENABLE_EmscriptenComponents)
    /* Controller API in Emscripten is not part of the SDL2 port */
    loader.registerAll<type_safety::type_list_t<sdl2::ControllerInput>>(
        container, ec);
    C_ERROR_CHECK(ec);
#endif

    appInfo.add("window:library", "SDL2 " + appInfo.get("sdl2:version"));
#elif defined(FEATURE_ENABLE_X11Component)
    loader.registerAll<x11::Services>(container, ec);
    C_ERROR_CHECK(ec);
    appInfo.add("window:library", "X11");
#elif defined(FEATURE_ENABLE_GLKitComponent)
    loader.registerAll<glkit::Services>(container, ec);
    C_ERROR_CHECK(ec);
    appInfo.add("window:library", "Apple GLKit");
#elif defined(FEATURE_ENABLE_ANativeComponent)
    loader.registerAll<anative::Services>(container, ec);
    C_ERROR_CHECK(ec);
    appInfo.add("window:library", "Android NativeActivity");
#elif defined(FEATURE_ENABLE_CogComponent)
    /* There is no window */
#elif defined(FEATURE_ENABLE_DispManXComponent)
    loader.registerAll<type_safety::type_list_t<
        comp_app::PtrNativeWindowInfoService,
        dispmanx::Windowing>>(container, ec);
    C_ERROR_CHECK(ec);
    appInfo.add("window:library", "DispManX");
#elif defined(FEATURE_ENABLE_EGLComponent)
    // For when there's no window creation necessary
    // For example NullWS on SGX
    loader.registerAll<type_safety::type_list_t<egl::Windowing>>(container, ec);
    C_ERROR_CHECK(ec);
    appInfo.add("window:library", "EGL Headless");
#else
#error No window manager
#endif

#if defined(FEATURE_ENABLE_EmscriptenComponents)
    loader.registerAll<type_safety::type_list_t<emscripten::ControllerInput>>(
        container, ec);
    C_ERROR_CHECK(ec);
#endif

    cVerbose(10, "Loading graphics context");
#if defined(SELECT_API_OPENGL)
    appInfo.add("graphics:library", "OpenGL");

    /* Selection of (E)GL context */
#if defined(COFFEE_EMSCRIPTEN) && 0
    loader.registerAll<emscripten::GLServices>(container, ec);
    C_ERROR_CHECK(ec);
    appInfo.add("gl:context", "Emscripten WebGL");
#elif defined(FEATURE_ENABLE_SDL2Components)
    loader.registerAll<sdl2::GLServices>(container, ec);
    appInfo.add("gl:context", "SDL2");
#elif defined(FEATURE_ENABLE_EGLComponent)
    loader.registerAll<egl::Services>(container, ec);
    C_ERROR_CHECK(ec);
    appInfo.add("gl:context", "EGL");
#else
#error No context manager
#endif

    cVerbose(10, "Loading graphics binding");
    /* Selection of GL binding */
#if GLEAM_USE_LINKED
    appInfo.add("gl:loader", "Linked");
#elif defined(FEATURE_ENABLE_GLADComponent_Dynamic)    \
    || defined(FEATURE_ENABLE_GLADComponent_ESDynamic) \
    || defined(FEATURE_ENABLE_GLADComponent_ES2Dynamic)
    loader.registerAll<glad::Services>(container, ec);
    C_ERROR_CHECK(ec);
    appInfo.add("gl:loader", "GLAD");
#else
#error No OpenGL/ES binding
#endif

#elif defined(FEATURE_ENABLE_CogComponent)
    loader.registerAll<cog::Services>(container, ec);
    C_ERROR_CHECK(ec);
#else
#error No graphics
#endif

#if defined(FEATURE_ENABLE_UIKitGestures)
    loader.registerAll<uikit::Services>(container, ec);
    C_ERROR_CHECK(ec);
#endif

    if constexpr(compile_info::debug_mode)
    {
        /* TODO: Conditionally load based on availability */
        loader.registerAll<detail::subsystem_list<
            comp_app::SysMemoryStats
#if defined(FEATURE_ENABLE_GLScreenshot_ES2Dynamic)   \
    || defined(FEATURE_ENABLE_GLScreenshot_Dynamic)   \
    || defined(FEATURE_ENABLE_GLScreenshot_ESDynamic) \
    || defined(FEATURE_ENABLE_GLScreenshot_ES)
            ,
            glscreenshot::ScreenshotProvider
#endif
            >>(container, ec);
        C_ERROR_CHECK(ec);
    }

    //     loader.registerAll<detail::subsystem_list<
    // #if defined(FEATURE_ENABLE_EmscriptenComponents)
    //         emscripten::BatteryProvider,
    // #else
    //         comp_app::SysBattery,
    // #endif
    //         comp_app::SysCPUTemp,
    //         comp_app::SysGPUTemp,
    //         comp_app::SysCPUClock>>(container, ec);
    //     C_ERROR_CHECK(ec);

    return;

    if(auto dinfo = container.service<DisplayInfo>())
        for(auto i : stl_types::Range<>(dinfo->count()))
        {
            auto idx = std::to_string(i);
            appInfo.add(
                "display:size:" + idx,
                std::to_string(
                    static_cast<int>(std::round(dinfo->diagonal(i)))));
            appInfo.add(
                "display:dpi:" + idx,
                std::to_string(static_cast<int>(std::round(dinfo->dpi(i)))));
            auto res = dinfo->size(i);
            appInfo.add(
                "display:resolution:" + idx,
                std::to_string(res.w) + "x" + std::to_string(res.h));
        }
    if(auto winfo = container.service<Windowing>())
        if(auto size = winfo->size(); size.w != 0)
            appInfo.add(
                "window:size",
                std::to_string(size.w) + "x" + std::to_string(size.h));
}

} // namespace comp_app

#include <coffee/core/printing/verbosity_level.h>
#include <platforms/profiling/jsonprofile.h>

namespace comp_app {

void PerformanceMonitor::start_restricted(proxy_type& p, time_point const& time)
{
    using namespace platform::profiling;

    auto timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
        time.time_since_epoch());

    json::CaptureMetrics(
        "Frametime",
        MetricVariant::Value,
        std::chrono::duration_cast<stl_types::Chrono::seconds_float>(
            time - m_prevFrame)
                .count()
            * 1000.f,
        timestamp);
    m_prevFrame = time;

    if(time < m_nextTime)
        return;

    m_nextTime = time + std::chrono::seconds(5);

    auto clock    = p.service<CPUClockProvider>();
    auto cpu_temp = p.service<CPUTempProvider>();
    auto gpu_temp = p.service<GPUTempProvider>();
    auto mem      = p.service<MemoryStatProvider>();
    auto battery  = p.service<BatteryProvider>();
    auto network  = p.service<NetworkStatProvider>();

    if(clock)
        for(auto i : Range<u32>(clock->threads()))
        {
            json::CaptureMetrics(
                "CPU frequency",
                MetricVariant::Value,
                C_FCAST<f32>(clock->frequency(i)),
                timestamp,
                i);
            json::CaptureMetrics(
                "CPU governor",
                MetricVariant::Value,
                C_CAST<u32>(clock->governor(i)),
                timestamp,
                i);
        }

    if(cpu_temp)
        json::CaptureMetrics(
            "CPU temperature",
            MetricVariant::Value,
            cpu_temp->value(),
            timestamp);
    if(gpu_temp)
        json::CaptureMetrics(
            "GPU temperature",
            MetricVariant::Value,
            gpu_temp->value(),
            timestamp);

    if(mem)
        json::CaptureMetrics(
            "Memory consumption",
            MetricVariant::Value,
            mem->resident(),
            timestamp);
    if(battery)
    {
        using PowerSource = interfaces::BatteryProvider::PowerSource;

        json::CaptureMetrics(
            "Battery level",
            MetricVariant::Value,
            battery->percentage(),
            timestamp);
        json::CaptureMetrics(
            "Power state",
            MetricVariant::Symbolic,
            battery->source() == PowerSource::Battery ? 1 : 0,
            timestamp);
    }

    if(network)
    {
        json::CaptureMetrics(
            "Network RX", MetricVariant::Value, network->received(), timestamp);
        json::CaptureMetrics(
            "Network TX",
            MetricVariant::Value,
            network->transmitted(),
            timestamp);
        json::CaptureMetrics(
            "Network connections",
            MetricVariant::Value,
            network->connections(),
            timestamp);
        network->reset_counters();
    }
}

void PerformanceMonitor::end_restricted(proxy_type &p, const time_point &time)
{
    capture_screenshot(p, time);
}

void PerformanceMonitor::capture_screenshot(
    proxy_type& p, time_point const& time)
{
    using namespace platform::profiling;
    using namespace Coffee::resource_literals;

    auto timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
        time.time_since_epoch());

    /* Disable screenshots on Emscripten,
     * there's better ways to debug framebuffers there */
    if constexpr(
        compile_info::debug_mode && !compile_info::platform::is_emscripten)
        do
        {
            auto screenshot = p.service<ScreenshotProvider>();

            if(!screenshot || time > m_nextScreenshot)
                break;

            screenshot->set_worker(m_worker_queue);

            m_nextScreenshot = time + std::chrono::seconds(10);
            auto pixels      = screenshot->pixels();

            /* If we're still waiting for the previous one, don't proceed */
            if(!pixels.valid())
                break;

            /* If not, set up JPG encoding + export to file and profiling */
            auto encode = [](dump_t* dump) {
                semantic::Bytes  encoded;
                stb::stb_error   ec;
                stb::image_const source = stb::image_const::From(
                    gsl::span(dump->data.data(), dump->data.size()),
                    dump->size,
                    4);

                if(!stb::SaveJPG(encoded, source, ec, 30))
                {
                    cWarning("Failed to dump screenshot: {0}", ec.message());
                    return semantic::Bytes();
                }
                return encoded;
            };
            auto export_file = [](semantic::Bytes const* data) {
                auto screenshot_file = "screenshot.jpg"_tmpfile;
                screenshot_file.data_ro
                    = semantic::BytesConst::ofBytes(data->data, data->size);
                FileCommit(
                    screenshot_file,
                    RSCA::WriteOnly | RSCA::Discard | RSCA::NewFile);
            };

            auto export_profile = [timestamp](semantic::Bytes const* data) {
                json::CaptureMetrics(
                    "Screenshots",
                    MetricVariant::Image,
                    b64::encode<byte_t>(data->view),
                    timestamp);
            };

            auto encoder
                = rq::dependent_task<dump_t, semantic::Bytes>::CreateProcessor(
                    std::move(pixels), std::move(encode));
            auto encoder_future
                = std::shared_future(encoder->output.get_future());
            auto file_exporter
                = rq::dependent_task<semantic::Bytes, void>::CreateSink(
                    encoder_future, std::move(export_file));
            auto profile_exporter
                = rq::dependent_task<semantic::Bytes, void>::CreateSink(
                    encoder_future, std::move(export_profile));

            /* JPG encoding + file export is put off-thread */
            auto current = rq::runtime_queue::GetCurrentQueue().value();
            rq::runtime_queue::Queue(m_worker_queue, std::move(encoder))
                .assume_value();
            rq::runtime_queue::Queue(m_worker_queue, std::move(file_exporter))
                .assume_value();
            rq::runtime_queue::Queue(current, std::move(profile_exporter))
                .assume_value();
        } while(false);

    json::CaptureMetrics(
        "VSYNC",
        MetricVariant::Marker,
        0,
        std::chrono::duration_cast<std::chrono::microseconds>(
            Profiler::clock::now().time_since_epoch()));
}

void PerformanceMonitor::load(AppLoadableService::entity_container&, app_error&)
{
    m_nextScreenshot = m_prevFrame
        = platform::profiling::Profiler::clock::now();
    if constexpr(
        compile_info::debug_mode && !compile_info::platform::is_emscripten)
        m_worker_queue
            = rq::runtime_queue::CreateNewThreadQueue("Profiling worker")
                  .assume_value();
}

void PerformanceMonitor::unload(
    AppLoadableService::entity_container&, app_error&)
{
}

} // namespace comp_app
