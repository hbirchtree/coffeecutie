#include <coffee/components/components.h>
#include <coffee/core/CApplication>
#include <coffee/core/task_queue/task.h>
#include <coffee/graphics/apis/CGLeamRHI>
#include <coffee/core/coffee.h>

#include <coffee/comp_app/bundle.h>
#include <coffee/comp_app/eventapp_wrapper.h>
#include <coffee/comp_app/gl_config.h>
#include <coffee/comp_app/subsystems.h>

#include <coffee/strings/geometry_types.h>
#include <coffee/strings/info.h>
#include <coffee/strings/libc_types.h>

namespace Coffee {
namespace Strings {

static CString to_string(comp_app::ControllerInput::controller_map const& box)
{
    return cStringFormat(
        "controller_map("
        "L({0},{1}):R({2},{3}):TRIG({4}:{5})"
        ":A={6}:B={7}:X={8}:Y={9}"
        ":BL={10}:BR={11}:SL={12}:SR={13}"
        ":START={14}:BACK={15}"
        ")",
        box.axes.e.l_x,
        box.axes.e.l_y,
        box.axes.e.r_x,
        box.axes.e.r_y,
        box.axes.e.t_l,
        box.axes.e.t_r,
        box.buttons.e.a,
        box.buttons.e.b,
        box.buttons.e.x,
        box.buttons.e.y,
        box.buttons.e.b_l,
        box.buttons.e.b_r,
        box.buttons.e.s_l,
        box.buttons.e.s_r,
        box.buttons.e.start,
        box.buttons.e.back);
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

    SetPrintingVerbosity(15);

    EntityContainer& e = comp_app::createContainer();

    comp_app::app_error ec;

    auto& loader =
        comp_app::AppLoader::register_service<comp_app::AppLoader>(e);

    comp_app::configureDefaults(loader);

    {
        auto& windowConf = loader.config<comp_app::WindowConfig>();
        auto& glConf     = loader.config<comp_app::GLConfig>();
        auto& ctlrConf   = loader.config<comp_app::ControllerConfig>();

        windowConf.size  = {1024, 768};
        windowConf.title = "Hello World";

        glConf.framebufferFmt = typing::pixels::PixFmt::RGBA8;

        glConf.profile = comp_app::GLConfig::Embedded;
        glConf.profile |= comp_app::GLConfig::Debug;
        glConf.version.major = 3;
        glConf.version.minor = 3;

        ctlrConf.options = comp_app::ControllerConfig::BackgroundInput;
    }

    comp_app::addDefaults(e, loader, ec);

    auto& inputs = *e.service<comp_app::BasicEventBus<CIEvent>>();

    inputs.addEventFunction<CIControllerAtomicUpdateEvent>(
        10, [](CIEvent&, CIControllerAtomicUpdateEvent* ev) {
            cDebug("Controller: {0} -> {1}", ev->controller, ev->connected);
        });

    auto displays = e.service<comp_app::DisplayInfo>();

    if(displays)
    {
        cDebug("{0}", displays->virtualSize());
        for(auto i : stl_types::Range<libc_types::u32>(displays->count()))
            cDebug("- {0}", displays->size(i));
    }

    C_ERROR_CHECK(ec);

    RuntimeQueue::CreateNewQueue("Main");

    runtime_queue_error rqec;
        RuntimeQueue::QueuePeriodic(
            RuntimeQueue::GetCurrentQueue(rqec),
            Chrono::milliseconds(100),
            [&e]() {
                auto dump  = e.service<comp_app::ControllerInput>();
                auto mouse = e.service<comp_app::MouseInput>();

                if(!dump || !mouse)
                    return;

                for(auto i : Range<u32>(dump->count()))
                    cDebug("Controller {0}: {1}", dump->name(i),
                    dump->state(i));

                cDebug(
                    "Mouse state: {0} {1}",
                    mouse->position(),
                    C_CAST<u32>(mouse->buttons()));
            },
            rqec);

    auto api = GFX::GetLoadAPI({});

    if(!api(true))
        return 1;

    comp_app::AutoExec<int, int>::addTo(
        e,
        [](int&, int*) {

        },
        [](int&, int*) {

        },
        [](int&, int*) {
            GFX::DefaultFramebuffer()->clear(0, Vecf4(0.5f, 0, 0, 1));
        },
        [](int&, int*) {

        });

    return comp_app::AutoExec<int, int>::exec(e);
}

COFFEE_APPLICATION_MAIN(coffee_main)