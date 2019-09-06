#include <coffee/comp_app/gl_config.h>
#include <coffee/comp_app/subsystems.h>
#include <coffee/components/components.h>
#include <coffee/core/CApplication>
#include <coffee/core/task_queue/task.h>
#include <coffee/graphics/apis/CGLeamRHI>
#include <coffee/graphics/apis/gleam/levels/gl_shared_include.h>
#include <coffee/sdl2_comp/app_components.h>

#include <coffee/strings/geometry_types.h>
#include <coffee/strings/info.h>
#include <coffee/strings/libc_types.h>

namespace Coffee {
namespace Strings {

static CString to_string(comp_app::ControllerInput::controller_map const& box)
{
    return cStringFormat(
        "mapping:{0}:{1}:{2}", box.buttons.e.a, box.axes.e.l_x, box.axes.e.l_y);
}

} // namespace Strings
} // namespace Coffee

#include <coffee/core/CDebug>

using namespace ::libc_types;

i32 coffee_main(i32, cstring_w*)
{
    using GFX = Coffee::RHI::GLEAM::GLEAM_API;

    using namespace Coffee;
    using namespace Coffee::Components;
    using namespace Coffee::Display;
    using namespace Coffee::Input;

    EntityContainer e;

    comp_app::app_error ec;

    auto& loader =
        comp_app::AppLoader::register_service<comp_app::AppLoader>(e);

    loader.addConfig(stl_types::MkUq<comp_app::WindowConfig>());
    loader.addConfig(stl_types::MkUq<comp_app::GLConfig>());

    auto& windowConf = loader.config<comp_app::WindowConfig>();
    auto& glConf     = loader.config<comp_app::GLConfig>();

    windowConf.size  = {1024, 768};
    windowConf.title = "Hello World";

    glConf.framebufferFmt = typing::pixels::PixFmt::RGBA8;
    //    glConf.profile |= comp_app::GLConfig::Debug;
    glConf.version.major = 3;
    glConf.version.minor = 3;

    using AppServices = comp_app::detail::TypeList<
        sdl2::Context,
        sdl2::Windowing,
        sdl2::DisplayInfo,
        sdl2::GLSwapControl,
        sdl2::GLContext,
        sdl2::GLFramebuffer,
        sdl2::ControllerInput,
        comp_app::BasicEventBus<CIEvent>,
        comp_app::BasicEventBus<Event>>;

    loader.load_all<AppServices>(e, ec);

    auto& inputs  = *e.service<comp_app::BasicEventBus<CIEvent>>();
    auto& context = *e.service<sdl2::Context>();

    inputs.addEventFunction<CIQuit>(0, [](CIEvent&, CIQuit* quit) {});
    inputs.addEventFunction<CIQuit>(10, [](CIEvent&, CIQuit* quit) {});
    inputs.addEventFunction<CIControllerAtomicUpdateEvent>(
        10, [](CIEvent&, CIControllerAtomicUpdateEvent* ev) {
            cDebug("Controller: {0} -> {1}", ev->controller, ev->connected);
        });

    auto& displays = *e.service<comp_app::DisplayInfo>();

    cDebug("{0}", displays.virtualSize());
    for(auto i : stl_types::Range<libc_types::u32>(displays.count()))
        cDebug("- {0}", displays.size(i));

    C_ERROR_CHECK(ec);

    RuntimeQueue::CreateNewQueue("Main");

    runtime_queue_error rqec;
    RuntimeQueue::QueuePeriodic(
        RuntimeQueue::GetCurrentQueue(rqec),
        Chrono::milliseconds(100),
        [&e]() {
            auto dump = e.service<sdl2::ControllerInput>();

            for(auto i : Range<u32>(dump->count()))
                cDebug("Controller {0}: {1}", dump->name(i), dump->state(i));
        },
        rqec);

    if(!gladLoadGL())
    {
        Coffee::cDebug("GL_VERSION: {0}", glGetString(GL_VERSION));
        return 1;
    }

    auto api = GFX::GetLoadAPI({});

    if(!api(false))
        return 1;

    while(!context.m_shouldClose)
    {
        GFX::DefaultFramebuffer()->clear(0, Coffee::Vecf4(0.2f));
        e.exec();
        RuntimeQueue::GetCurrentQueue(rqec)->executeTasks();
    }

    loader.unload_all<AppServices>(e, ec);

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
