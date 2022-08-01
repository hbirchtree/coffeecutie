#include "renderer.h"
#include <coffee/core/CApplication>
#include <coffee/core/coffee.h>
#include <coffee/core/input/eventhandlers.h>
#include <coffee/core/input/standard_input_handlers.h>
#include <coffee/core/task_queue/task.h>
#include <coffee/interfaces/full_launcher.h>

#include <coffee/asio/net_profiling.h>

i32 coffee_main(i32, cstring_w*)
{
    using namespace EventHandlers;
    using namespace StandardInput;

    using Display::Event;
    using Input::CIEvent;

#if defined(FEATURE_ENABLE_ASIO)
    C_UNUSED(auto _ = Net::RegisterProfiling());
#endif

    auto& entities = comp_app::createContainer();
    auto& loader = comp_app::AppLoader::register_service(entities);

    comp_app::configureDefaults(loader);

    {
        auto& window = loader.config<comp_app::WindowConfig>();
        window.title = "Hello World";
    }

    comp_app::app_error ec;
    comp_app::addDefaults(entities, loader, ec);
    C_ERROR_CHECK(ec)

    auto& input = *entities.service<comp_app::BasicEventBus<CIEvent>>();

//    input.addEventHandler(input.default_prio, EHandle<Event>::MkHandler(WindowResize<>()));
    input.addEventHandler(1024, ExitOn<OnKey<Input::CK_Escape>>([&entities]() {
        entities.service<comp_app::Windowing>()->close();
    }));

    comp_app::AppContainer<RendererState>::addTo(
        entities,
        SetupRendering,
        RendererLoop,
        RendererCleanup);

    return comp_app::ExecLoop<comp_app::BundleData>::exec(entities);
}

COFFEE_APPLICATION_MAIN(coffee_main)
