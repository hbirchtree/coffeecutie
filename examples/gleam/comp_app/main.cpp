#include <coffee/comp_app/subsystems.h>
#include <coffee/components/components.h>
#include <coffee/core/CApplication>
#include <coffee/sdl2_comp/app_components.h>

using namespace ::libc_types;

i32 coffee_main(i32, cstring_w*)
{
    using namespace Coffee::Components;
    using namespace Coffee::Display;
    using namespace Coffee::Input;

    EntityContainer e;

    comp_app::app_error ec;

    //    auto& window =
    //    comp_app::Windowing::register_service<sdl2::Windowing>(e); auto&
    //    context = sdl2::Context::register_service<sdl2::Context>(e); auto&
    //    inputs =
    //    comp_app::EventBus<CIEvent>::register_service<comp_app::BasicEventBus<CIEvent>>(e);
    //    comp_app::EventBus<Event>::register_service<comp_app::BasicEventBus<Event>>(e);

    auto& loader =
        comp_app::AppLoader::register_service<comp_app::AppLoader>(e);

    loader.load_all<comp_app::detail::TypeList<
        sdl2::Context,
        sdl2::Windowing,
        sdl2::DisplayInfo,
        comp_app::BasicEventBus<CIEvent>,
        comp_app::BasicEventBus<Event>>>(e, ec);

    auto& inputs  = *e.service<comp_app::BasicEventBus<CIEvent>>();
    auto& context = *e.service<sdl2::Context>();

    inputs.addEventFunction<CIQuit>(0, [](CIEvent&, CIQuit* quit) {});
    inputs.addEventFunction<CIQuit>(10, [](CIEvent&, CIQuit* quit) {});

    //    context.load(e, ec);
    //    window.load(e, ec);

    C_ERROR_CHECK(ec);

    while(!context.m_shouldClose)
        e.exec();

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
