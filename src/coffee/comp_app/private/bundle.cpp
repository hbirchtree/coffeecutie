#include <coffee/comp_app/bundle.h>

#include <coffee/comp_app/app_events.h>
#include <coffee/comp_app/eventapp_wrapper.h>
#include <coffee/comp_app/gl_config.h>
#include <coffee/comp_app/performance_monitor.h>
#include <coffee/core/CProfiling>
#include <coffee/core/base_state.h>
#include <coffee/core/task_queue/task.h>
#include <coffee/core/types/display/event.h>
#include <coffee/core/types/input/event_types.h>
#include <coffee/foreign/foreign.h>
#include <coffee/image/cimage.h>
#include <peripherals/libc/signals.h>
#include <peripherals/stl/string_ops.h>
#include <platforms/environment.h>

#include <platforms/stacktrace.h>

#include <coffee/comp_app/stat_providers.h>

#include <coffee/strings/libc_types.h>

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

#if defined(FEATURE_ENABLE_GLADComponent_Dynamic) ||   \
    defined(FEATURE_ENABLE_GLADComponent_ESDynamic) || \
    defined(FEATURE_ENABLE_GLADComponent_ES2Dynamic)
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

#if defined(FEATURE_ENABLE_GLScreenshot)
#include <glscreenshot/screenshot.h>
#endif

#if defined(FEATURE_ENABLE_GLKitComponent) || \
    defined(FEATURE_ENABLE_ANativeComponent) || defined(COFFEE_EMSCRIPTEN)
#define USES_LINKED_GL 1
#endif

#if defined(FEATURE_ENABLE_GLADComponent_Dynamic) ||    \
    defined(FEATURE_ENABLE_GLADComponent_ESDynamic) ||  \
    defined(FEATURE_ENABLE_GLADComponent_ES2Dynamic) || \
    defined(FEATURE_ENABLE_EGLComponent) ||             \
    defined(FEATURE_ENABLE_SDL2Components) || USES_LINKED_GL
#define USES_GL 1
#endif

namespace comp_app {

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

#if defined(COFFEE_EMSCRIPTEN)
void emscripten_loop()
{
    static bool isLoaded = false;

    //    try
    //    {
    if(!isLoaded)
    {
        CoffeeEventHandle(nullptr, CoffeeHandle_Setup);
        isLoaded = true;
    }

    CoffeeEventHandle(nullptr, CoffeeHandle_Loop);
    //    } catch(std::exception const& e)
    //    {
    //        emscripten_log(
    //            EM_LOG_ERROR,
    //            "Exception encountered: %s: %s",
    //            platform::stacktrace::demangle::name(typeid(e).name()).c_str(),
    //            e.what());
    //        emscripten_pause_main_loop();
    //    }
}
#endif

struct app_loadable_matcher
{
    static bool match(Coffee::Components::SubsystemBase* subsys)
    {
        return C_DCAST<AppLoadableService>(subsys) != nullptr;
    }
};

detail::EntityContainer& createContainer()
{
    static stl_types::ShPtr<detail::EntityContainer> container;

    if(container)
        return *container;

    container = stl_types::MkShared<detail::EntityContainer>();

    using namespace Coffee;

    coffee_event_handling_data = container.get();
    CoffeeEventHandle          = [](void*, int event) {
        using namespace Coffee;

        runtime_queue_error ec;

        switch(event)
        {
        case CoffeeHandle_Setup: {
            DProfContext _("Bundle::Setup");

            app_error ec;
            for(auto& service : container->services_with<AppLoadableService>())
            {
                if(C_DCAST<AppMain>(&service))
                    continue;
                service.load(*container, ec);
                C_ERROR_CHECK(ec)
            }

            auto eventMain = container->service<AppMain>();
            if(!eventMain)
                break;
            eventMain->load(*container, ec);
            C_ERROR_CHECK(ec)
            break;
        }
        case CoffeeHandle_Loop: {
            DProfContext _("Bundle::Loop");

            container->exec();

            auto queue = RuntimeQueue::GetCurrentQueue(ec);
            if(queue)
                queue->executeTasks();
            break;
        }
        case CoffeeHandle_Cleanup: {
            DProfContext _("Bundle::Cleanup");

            app_error appec;
            auto      services = container->services_with<AppLoadableService>(
                Components::reverse_query);

            for(auto& service : services)
                service.unload(*container, appec);
            break;
        }
        default: {
            DProfContext _("Bundle::AppEvent");
            auto         app_bus = container->service<EventBus<AppEvent>>();

            if(!app_bus)
                break;

            AppEvent appevent;
            appevent.type = AppEvent::None;
            union
            {
                LifecycleEvent  lifecycle;
                NavigationEvent navi;
            };
            switch(event)
            {
            case CoffeeHandle_LowMem:
            case CoffeeHandle_IsTerminating:
            case CoffeeHandle_IsBackground:
            case CoffeeHandle_IsForeground:
            case CoffeeHandle_TransBackground:
            case CoffeeHandle_TransForeground:
                appevent.type = AppEvent::LifecycleEvent;
                break;
            }

            switch(event)
            {
            case CoffeeHandle_LowMem:
                lifecycle.lifecycle_type = LifecycleEvent::LowMemory;
                break;
            case CoffeeHandle_IsTerminating:
                lifecycle.lifecycle_type = LifecycleEvent::Terminate;
                break;
            case CoffeeHandle_IsBackground:
                lifecycle.lifecycle_type = LifecycleEvent::Background;
                break;
            case CoffeeHandle_IsForeground:
                lifecycle.lifecycle_type = LifecycleEvent::Foreground;
                break;
            case CoffeeHandle_TransBackground:
                lifecycle.lifecycle_type = LifecycleEvent::WillEnterBackground;
                break;
            case CoffeeHandle_TransForeground:
                lifecycle.lifecycle_type = LifecycleEvent::WillEnterForeground;
                break;
            }
            switch(event)
            {
            case CoffeeHandle_LowMem:
            case CoffeeHandle_IsTerminating:
            case CoffeeHandle_IsBackground:
            case CoffeeHandle_IsForeground:
            case CoffeeHandle_TransBackground:
            case CoffeeHandle_TransForeground:
                app_bus->process(appevent, &lifecycle);
                break;
            }
        }
        }
    };
    CoffeeEventHandleNA = [](void*, int, void*, void*, void*) {};

    if constexpr(
        compile_info::platform::is_unix &&
        !compile_info::platform::is_emscripten)
        libc::signal::install(libc::signal::sig::interrupt, [](int) {
            if(!container)
                return;

            auto windowing = container->service<Windowing>();

            if(!windowing)
                return;

            windowing->close();
        });

    return *container;
}

void configureDefaults(AppLoader& loader)
{
    Coffee::DProfContext _("comp_app::configureDefaults");

    loader.addConfigs<
        detail::
            TypeList<WindowConfig, ControllerConfig, GraphicsBindingConfig>>();

#if USES_GL
    loader.addConfigs<detail::TypeList<GLConfig>>();

    auto& glConfig          = loader.config<GLConfig>();
    glConfig.framebufferFmt = PixFmt::RGBA8;
    glConfig.depthFmt       = PixFmt::Depth24Stencil8;

#if !defined(GLEAM_USE_LINKED)
    glConfig.framebufferFmt = PixFmt::SRGB8A8;
#endif

#if defined(GLEAM_USE_LINKED)
    glConfig.framebufferFmt = PixFmt::RGB565;
    glConfig.profile        = GLConfig::Embedded;
    glConfig.version.major  = 3;
    glConfig.version.minor  = 0;
#else
    glConfig.profile       = GLConfig::Core;
    glConfig.version.major = 3;
    glConfig.version.minor = 3;
#endif
#endif

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
}

void addDefaults(
    AppLoadableService::entity_container& container,
    AppLoader&                            loader,
    app_error&                            ec)
{
    Coffee::ProfContext _("comp_app::addDefaults");

    loader.loadAll<subsystem_list<
        BasicEventBus<Coffee::Input::CIEvent>,
        BasicEventBus<Coffee::Display::Event>,
        BasicEventBus<AppEvent>,
        DefaultAppInfo>>(container, ec);

    auto& appInfo = *container.service<AppInfo>();

    if constexpr(compile_info::debug_mode && compile_info::profiler::enabled)
    {
        loader.loadAll<type_safety::type_list_t<PerformanceMonitor>>(
            container, ec);
        C_ERROR_CHECK(ec);
    }

    /* Selection of window/event manager */
#if defined(FEATURE_ENABLE_SDL2Components)
    sdl2::GLContext::register_service<sdl2::GLContext>(container);
    loader.loadAll<sdl2::Services>(container, ec);
    C_ERROR_CHECK(ec);

#if !defined(FEATURE_ENABLE_EmscriptenComponents)
    /* Controller API in Emscripten is not part of the SDL2 port */
    loader.loadAll<type_safety::type_list_t<sdl2::ControllerInput>>(
        container, ec);
    C_ERROR_CHECK(ec);
#endif

    appInfo.add("window:library", "SDL2 " + appInfo.get("sdl2:version"));
#elif defined(FEATURE_ENABLE_X11Component)
    loader.loadAll<x11::Services>(container, ec);
    C_ERROR_CHECK(ec);
    appInfo.add("window:library", "X11");
#elif defined(FEATURE_ENABLE_GLKitComponent)
    loader.loadAll<glkit::Services>(container, ec);
    C_ERROR_CHECK(ec);
    appInfo.add("window:library", "Apple GLKit");
#elif defined(FEATURE_ENABLE_ANativeComponent)
    loader.loadAll<anative::Services>(container, ec);
    C_ERROR_CHECK(ec);
    appInfo.add("window:library", "Android NativeActivity");
#elif defined(FEATURE_ENABLE_CogComponent)
    /* There is no window */
#elif defined(COFFEE_BEAGLEBONE)
    loader.loadAll<type_safety::type_list_t<egl::Windowing>>(container, ec);
    C_ERROR_CHECK(ec);
    appInfo.add("window:library", "EGL Headless");
#else
#error No window manager
#endif

#if defined(FEATURE_ENABLE_EmscriptenComponents)
    loader.loadAll<type_safety::type_list_t<emscripten::ControllerInput>>(
        container, ec);
    C_ERROR_CHECK(ec);
#endif

#if defined(SELECT_API_OPENGL)
    appInfo.add("graphics:library", "OpenGL");

    /* Selection of (E)GL context */
#if defined(FEATURE_ENABLE_SDL2Components)
    sdl2::GLSwapControl::register_service<sdl2::GLSwapControl>(container);
    sdl2::GLFramebuffer::register_service<sdl2::GLFramebuffer>(container);
    container.service<sdl2::GLContext>()->load(container, ec);
    C_ERROR_CHECK(ec);
    container.service<sdl2::GLFramebuffer>()->load(container, ec);
    C_ERROR_CHECK(ec);
    appInfo.add("gl:context", "SDL2");
#elif defined(FEATURE_ENABLE_EGLComponent)
    loader.loadAll<egl::Services>(container, ec);
    C_ERROR_CHECK(ec);
    appInfo.add("gl:context", "EGL");
#else
#error No context manager
#endif

    /* Selection of GL binding */
#if GLEAM_USE_LINKED
    appInfo.add("gl:loader", "Linked");
#elif defined(FEATURE_ENABLE_GLADComponent_Dynamic) || \
    defined(FEATURE_ENABLE_GLADComponent_ESDynamic) || \
    defined(FEATURE_ENABLE_GLADComponent_ES2Dynamic)
    loader.loadAll<glad::Services>(container, ec);
    C_ERROR_CHECK(ec);
    appInfo.add("gl:loader", "GLAD");
#else
#error No OpenGL/ES binding
#endif

#elif defined(FEATURE_ENABLE_CogComponent)
    loader.loadAll<cog::Services>(container, ec);
    C_ERROR_CHECK(ec);
#else
#error No graphics
#endif

#if defined(FEATURE_ENABLE_UIKitGestures)
    loader.loadAll<uikit::Services>(container, ec);
    C_ERROR_CHECK(ec);
#endif

    if constexpr(compile_info::debug_mode)
    {
        /* TODO: Conditionally load based on availability */
        loader.loadAll<detail::subsystem_list<
            comp_app::SysBattery,
            comp_app::SysCPUTemp,
            comp_app::SysGPUTemp,
            comp_app::SysCPUClock,
            comp_app::SysMemoryStats>>(container, ec);
        C_ERROR_CHECK(ec);

#if defined(FEATURE_ENABLE_GLScreenshot)
        loader.loadAll<detail::TypeList<glscreenshot::ScreenshotProvider>>(
            container, ec);
        C_ERROR_CHECK(ec);
#endif
    }
}

} // namespace comp_app

#include <coffee/core/printing/verbosity_level.h>
#include <platforms/profiling/jsonprofile.h>

namespace comp_app {

void PerformanceMonitor::start_restricted(proxy_type& p, time_point const& time)
{
    using namespace platform::profiling;

    auto timestamp =
        Chrono::duration_cast<Chrono::microseconds>(time.time_since_epoch());

    json::CaptureMetrics(
        "Frametime",
        MetricVariant::Value,
        Chrono::duration_cast<Chrono::seconds_float>(time - m_prevFrame)
                .count() *
            1000.f,
        timestamp);
    m_prevFrame = time;

    if(time < m_nextTime)
        return;

    m_nextTime = time + Coffee::Chrono::seconds(5);

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
        json::CaptureMetrics(
            "Battery level",
            MetricVariant::Value,
            battery->percentage(),
            timestamp);
        json::CaptureMetrics(
            "Power state",
            MetricVariant::Symbolic,
            battery->source() ==
                    interfaces::BatteryProvider::PowerSource::Battery
                ? 1
                : 0,
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

void PerformanceMonitor::end_restricted(proxy_type& p, time_point const& time)
{
    using namespace platform::profiling;

    auto timestamp =
        Chrono::duration_cast<Chrono::microseconds>(time.time_since_epoch());

    if constexpr(compile_info::debug_mode)
    {
        auto screenshot = p.service<ScreenshotProvider>();

        if(screenshot && time > m_nextScreenshot)
        {
            using namespace Coffee;

            m_nextScreenshot        = time + Chrono::seconds(10);
            auto             pixels = screenshot->pixels();
            Bytes            encoded;
            stb::stb_error   ec;
            stb::image_const source = stb::image_const::From(
                pixels.as<Bytes::value_type>(), screenshot->size(), 3);

            stb::SaveJPG(encoded, source, ec, 50);
            auto screenshot_file = "screenshot.jpg"_tmpfile;

            FileOpenMap(
                screenshot_file,
                encoded.size,
                RSCA::ReadWrite | RSCA::Persistent | RSCA::NewFile |
                    RSCA::Discard);
            if(encoded.size == screenshot_file.size)
                memcpy(
                    screenshot_file.data, encoded.data, screenshot_file.size);
            FileUnmap(screenshot_file);

            json::CaptureMetrics(
                "Screenshots",
                MetricVariant::Image,
                b64::encode<byte_t>(encoded),
                timestamp);
        }
    }

    json::CaptureMetrics(
        "VSYNC",
        MetricVariant::Marker,
        0,
        Chrono::duration_cast<Chrono::microseconds>(
            Profiler::clock::now().time_since_epoch()));
}

void PerformanceMonitor::load(AppLoadableService::entity_container&, app_error&)
{
    m_prevFrame = platform::profiling::Profiler::clock::now();
}

void PerformanceMonitor::unload(
    AppLoadableService::entity_container&, app_error&)
{
}

} // namespace comp_app
