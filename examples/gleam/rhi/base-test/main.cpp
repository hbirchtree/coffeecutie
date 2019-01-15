#include "renderer.h"
#include <coffee/core/CApplication>
#include <coffee/core/coffee.h>
#include <coffee/core/input/eventhandlers.h>
#include <coffee/core/input/standard_input_handlers.h>
#include <coffee/core/task_queue/task.h>
#include <coffee/interfaces/full_launcher.h>

#if defined(FEATURE_ENABLE_ASIO)
#include <coffee/asio/net_profiling.h>
#endif

using EDATA = EventLoopData<CDRenderer, RendererState>;

i32 coffee_main(i32, cstring_w*)
{
    using namespace EventHandlers;
    using namespace StandardInput;

#if defined(FEATURE_ENABLE_ASIO)
    Net::RegisterProfiling();
#endif

    CString err;
    return AutoExec<GLM, CDRenderer, RendererState>(
        [](CDRenderer& r, RendererState* d, Display::Properties& props) {
            props.gl.flags |= GL::Properties::GLDebug | GL::Properties::GLVSync;

            /* Install some standard event handlers */
            r.installEventHandler({EscapeCloseWindow<CDRenderer>, nullptr, &r});
            r.installEventHandler(
                {WindowManagerCloseWindow<CDRenderer>, nullptr, &r});
            r.installEventHandler({ResizeWindowUniversal<GLM>, nullptr, &r});
            r.installEventHandler(
                {WindowManagerFullscreen<CDRenderer>, nullptr, &r});
            r.installEventHandler(
                {StandardCamera<CGCamera>, nullptr, &d->camera_cnt.get()});
        },
        SetupRendering,
        RendererLoop,
        RendererCleanup);
}

COFFEE_APPLICATION_MAIN(coffee_main)
