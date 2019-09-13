#include <coffee/comp_app/bundle.h>

#include <coffee/comp_app/eventapp_wrapper.h>
#include <coffee/core/task_queue/task.h>
#include <coffee/core/types/display/event.h>
#include <coffee/core/types/input/event_types.h>
#include <coffee/foreign/foreign.h>
#include <platforms/environment.h>

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
#include <coffee/sdl2_comp/app_components.h>
#endif

#if defined(FEATURE_ENABLE_GLKitComponent)
#include <coffee/glkit/glkit_comp.h>
#endif

#if defined(FEATURE_ENABLE_ANativeComponent)
#include <coffee/anative/anative_comp.h>
#endif

namespace comp_app {

detail::EntityContainer& createContainer()
{
    static stl_types::ShPtr<detail::EntityContainer> container;

    if(!container)
        container = stl_types::MkShared<detail::EntityContainer>();

    using namespace Coffee;

    coffee_event_handling_data = container.get();
    CoffeeEventHandle          = [](void*, int event) {
        runtime_queue_error ec;

        switch(event)
        {
        case CoffeeHandle_Setup:
        {
            app_error ec;
            container->service<EventMain>()->load(*container, ec);
            C_ERROR_CHECK(ec);
            break;
        }
        case CoffeeHandle_Loop:
        {
            container->exec();

            RuntimeQueue::GetCurrentQueue(ec)->executeTasks();
            C_ERROR_CHECK(ec);
            break;
        }
        case CoffeeHandle_Cleanup:
        {
            /* TODO: Unload all services */
            container->services_with<AppLoadableService>();
            break;
        }
        }
    };
    CoffeeEventHandleNA = [](void*, int event, void*, void*, void*) {

    };

    return *container;
}

void configureDefaults(AppLoader& loader)
{
    loader.addConfigs<
        detail::
            TypeList<WindowConfig, ControllerConfig, GraphicsBindingConfig>>();

#if defined(FEATURE_ENABLE_GLADComponent)
    loader.addConfigs<detail::TypeList<GLConfig>>();

    auto& glConfig          = loader.config<GLConfig>();
    glConfig.profile        = GLConfig::Core;
    glConfig.framebufferFmt = PixFmt::RGBA8;
    glConfig.depthFmt       = PixFmt::Depth24Stencil8;
#endif

#if defined(FEATURE_ENABLE_X11Component)
    loader.addConfigs<detail::TypeList<x11::X11Config>>();
#if defined(COFFEE_MAEMO)
    auto& x11Config  = loader.config<x11::X11Config>();
    x11Config.hildon = true;
#endif
#endif

    auto& window = loader.config<WindowConfig>();

    window.title = {};
    window.size  = {1280, 720};
}

void addDefaults(
    AppLoadableService::entity_container& container,
    AppLoader&                            loader,
    app_error&                            ec)
{
    loader.loadAll<detail::TypeList<
        BasicEventBus<Coffee::Input::CIEvent>,
        BasicEventBus<Coffee::Display::Event>>>(container, ec);

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
#else
#error No window manager
#endif

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
#if defined(FEATURE_ENABLE_AppDelegate) || \
    defined(FEATURE_ENABLE_ANativeComponent) || defined(COFFEE_EMSCRIPTEN)
    loader.loadAll<detail::TypeList<LinkedGraphicsBinding>>(container, ec);
#elif defined(FEATURE_ENABLE_GLADComponent)
    loader.loadAll<glad::Services>(container, ec);
#else
#error No OpenGL/ES binding
#endif
}

} // namespace comp_app