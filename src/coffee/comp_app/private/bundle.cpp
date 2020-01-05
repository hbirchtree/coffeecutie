#include <coffee/comp_app/bundle.h>

#include <coffee/comp_app/app_events.h>
#include <coffee/comp_app/eventapp_wrapper.h>
#include <coffee/comp_app/gl_config.h>
#include <coffee/core/CProfiling>
#include <coffee/core/base_state.h>
#include <coffee/core/task_queue/task.h>
#include <coffee/core/types/display/event.h>
#include <coffee/core/types/input/event_types.h>
#include <coffee/foreign/foreign.h>
#include <platforms/environment.h>

#if defined(COFFEE_EMSCRIPTEN)
#include <emscripten/emscripten.h>
#endif

#if defined(FEATURE_ENABLE_EGLComponent)
#include <coffee/egl/egl_comp.h>
#endif

#if defined(FEATURE_ENABLE_X11Component)
#include <coffee/x11/x11_comp.h>
#endif

#if defined(FEATURE_ENABLE_GLADComponent)
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

#if defined(FEATURE_ENABLE_GLKitComponent) || \
    defined(FEATURE_ENABLE_ANativeComponent) || defined(COFFEE_EMSCRIPTEN)
#define USES_LINKED_GL 1
#endif

#if defined(FEATURE_ENABLE_GLADComponent) || \
    defined(FEATURE_ENABLE_SDL2Components) || USES_LINKED_GL
#define USES_GL 1
#endif

namespace comp_app {

#if defined(COFFEE_EMSCRIPTEN)
void emscripten_loop()
{
    static bool isLoaded = false;

    try
    {
        if(!isLoaded)
        {
            CoffeeEventHandle(nullptr, CoffeeHandle_Setup);
            isLoaded = true;
        }

        CoffeeEventHandle(nullptr, CoffeeHandle_Loop);
    } catch(std::exception const& e)
    {
        emscripten_log(EM_LOG_ERROR, "Exception encountered: %s", e.what());
        emscripten_pause_main_loop();
    }
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
        case CoffeeHandle_Setup:
        {
            app_error ec;
            for(auto& service : container->services_with<AppLoadableService>())
            {
                if(C_DCAST<AppMain>(&service))
                    continue;
                service.load(*container, ec);
                C_ERROR_CHECK(ec)
            }

            auto eventMain = container->service<AppMain>();
            C_PTR_CHECK(eventMain)
            eventMain->load(*container, ec);
            C_ERROR_CHECK(ec)
            break;
        }
        case CoffeeHandle_Loop:
        {
            container->exec();

            auto queue = RuntimeQueue::GetCurrentQueue(ec);
            if(queue)
                queue->executeTasks();
            break;
        }
        case CoffeeHandle_Cleanup:
        {
            /* TODO: Unload all services */
            app_error appec;
            for(auto& service : container->services_with<AppLoadableService>())
                service.unload(*container, appec);
            break;
        }
        default:
        {
            auto&    app_bus = *container->service<EventBus<AppEvent>>();
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
                app_bus.process(appevent, &lifecycle);
                break;
            }
        }
        }
    };
    CoffeeEventHandleNA = [](void*, int event, void*, void*, void*) {

    };

    return *container;
}

void configureDefaults(AppLoader& loader)
{
    Coffee::ProfContext _("comp_app::configureDefaults");

    loader.addConfigs<
        detail::
            TypeList<WindowConfig, ControllerConfig, GraphicsBindingConfig>>();

#if USES_GL
    loader.addConfigs<detail::TypeList<GLConfig>>();

    auto& glConfig          = loader.config<GLConfig>();
    glConfig.framebufferFmt = PixFmt::RGBA8;
    glConfig.depthFmt       = PixFmt::Depth24Stencil8;

#if defined(COFFEE_LINKED_GLES)
    glConfig.profile       = GLConfig::Embedded;
    glConfig.version.major = 3;
    glConfig.version.minor = 0;
#else
    glConfig.profile       = GLConfig::Core;
    glConfig.version.major = 3;
    glConfig.version.minor = 3;
#endif

#if defined(COFFEE_EMSCRIPTEN)
    glConfig.version.major = 2;
    glConfig.version.minor = 0;
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

    loader.loadAll<detail::TypeList<
        BasicEventBus<Coffee::Input::CIEvent>,
        BasicEventBus<Coffee::Display::Event>,
        BasicEventBus<AppEvent>>>(container, ec);

    /* Selection of window/event manager */
#if defined(FEATURE_ENABLE_SDL2Components)
    sdl2::GLContext::register_service<sdl2::GLContext>(container);
    loader.loadAll<sdl2::Services>(container, ec);
#elif defined(FEATURE_ENABLE_X11Component)
    loader.loadAll<x11::Services>(container, ec);
#elif defined(FEATURE_ENABLE_GLKitComponent)
    loader.loadAll<glkit::Services>(container, ec);
#elif defined(FEATURE_ENABLE_ANativeComponent)
    loader.loadAll<anative::Services>(container, ec);
#elif defined(FEATURE_ENABLE_CogComponent)
    /* There is no window */
#else
#error No window manager
#endif

#if defined(SELECT_API_OPENGL)
    /* Selection of (E)GL context */
#if defined(FEATURE_ENABLE_SDL2Components)
    sdl2::GLSwapControl::register_service<sdl2::GLSwapControl>(container);
    sdl2::GLFramebuffer::register_service<sdl2::GLFramebuffer>(container);
    container.service<sdl2::GLContext>()->load(container, ec);
    container.service<sdl2::GLFramebuffer>()->load(container, ec);
#elif defined(FEATURE_ENABLE_EGLComponent)
    loader.loadAll<egl::Services>(container, ec);
#else
#error No context manager
#endif

    /* Selection of GL binding */
#if defined(FEATURE_ENABLE_GLKitComponent) ||                                 \
    defined(FEATURE_ENABLE_ANativeComponent) || defined(COFFEE_EMSCRIPTEN) || \
    defined(COFFEE_RASPBERRYPI)
    loader.loadAll<detail::TypeList<LinkedGraphicsBinding>>(container, ec);
#elif defined(FEATURE_ENABLE_GLADComponent)
    loader.loadAll<glad::Services>(container, ec);
#else
#error No OpenGL/ES binding
#endif

#elif defined(FEATURE_ENABLE_CogComponent)

    loader.loadAll<cog::Services>(container, ec);

#else
#error No graphics
#endif

#if defined(FEATURE_ENABLE_UIKitGestures)
    loader.loadAll<uikit::Services>(container, ec);
#endif
}

} // namespace comp_app
