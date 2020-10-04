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

    CString err;
    return AutoExec<GLM, CDRenderer, RendererState>(
        [](ShPtr<CDRenderer>    r,
           ShPtr<RendererState>,
           Display::Properties& props) {
            props.gl.flags |= GL::Properties::GLDebug | GL::Properties::GLVSync;

            /* Install some standard event handlers */
            r->installEventHandler(
                EHandle<Event>::MkHandler(WindowResize<GLM>()));
            r->installEventHandler(EHandle<CIEvent>::MkHandler(
                ExitOn<OnKey<Input::CK_Escape>>(r->window())));
            r->installEventHandler(
                EHandle<CIEvent>::MkHandler(ExitOn<OnQuit>(r->window())));
            r->installEventHandler(EHandle<CIEvent>::MkHandler(
                FullscreenOn<AnyIKey<
                    KeyCombo<
                        CK_EnterNL,
                        CIKeyEvent::KeyModifiers::RAltModifier>,
                    KeyCombo<CK_F11>>>(r->window())));
        },
        SetupRendering,
        RendererLoop,
        RendererCleanup);
}

COFFEE_APPLICATION_MAIN(coffee_main)
