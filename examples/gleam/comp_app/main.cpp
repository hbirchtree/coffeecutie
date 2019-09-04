#include <coffee/core/CApplication>
#include <coffee/sdl2_comp/app_components.h>
#include <coffee/components/components.h>

using namespace ::libc_types;

i32 coffee_main(i32, cstring_w*)
{
    using namespace Coffee::Components;

    EntityContainer e;

    comp_app::app_error ec;

    auto& window = comp_app::Windowing::register_service<sdl2::Windowing>(e);
    auto& context = sdl2::Context::register_service<sdl2::Context>(e);

    context.load(e, ec);
    window.load(e, ec);

    C_ERROR_CHECK(ec);

    while(!context.m_shouldClose)
        e.exec();

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
