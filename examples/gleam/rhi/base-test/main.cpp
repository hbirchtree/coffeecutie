#include <coffee/core/CApplication>
#include <coffee/sdl2/CSDL2Dialog>
#include <coffee/core/input/eventhandlers.h>
#include <coffee/core/coffee.h>
#include "renderer.h"
#include <coffee/core/input/standard_input_handlers.h>
#include <coffee/core/task_queue/task.h>

void ExitOnBackground(void* user_ptr, CDEvent const& ev, c_cptr data)
{
//    auto r = C_CAST<CDRenderer*>(user_ptr);

    cDebug("Caught window event: {0}", C_CAST<uint32>(ev.type));

    if(ev.type == CDEvent::Focus)
    {
        auto state = C_CAST<CDFocusEvent const*>(data);
        if(state->mod == CDFocusEvent::Leave)
            exit(0);
    }
}

using EDATA = EventLoopData<CDRenderer, RendererState>;

int32 coffee_main(int32, cstring_w*)
{
    /* Set a prefix from which resources are fetched */
    CResources::FileResourcePrefix("sample_data/eye-demo/");
//    SetPrintingVerbosity(8);
//    GotoApplicationDir();

    /*Moving on to regular rendering*/
    Profiler::PushContext("Root");
//    renderer->installEventHandler({ExitOnBackground, nullptr, renderer});

    Profiler::Profile("Object creation");

    /* Set up the window visual */
    CDProperties props = GetDefaultVisual<RHI::GLEAM::GLEAM_API>();

    cDebug("Property object size: {0}", sizeof(props));

    props.flags ^= CDProperties::Resizable;
    props.gl.flags |= GLProperties::GLDebug;
    
    EDATA* loop = new EDATA{
        new CDRenderer,
        new RendererState,
        SetupRendering,
        RendererLoop,
        RendererCleanup,
        0, {}
        };
    
    auto renderer = loop->renderer;
    
    /* Install some standard event handlers */
    renderer->installEventHandler({EventHandlers::EscapeCloseWindow<CDRenderer>,
                                   nullptr, renderer});
    renderer->installEventHandler({EventHandlers::WindowManagerCloseWindow<CDRenderer>,
                                   nullptr, renderer});
    renderer->installEventHandler({EventHandlers::ResizeWindowUniversal<GLM>,
                                   nullptr, renderer});
    renderer->installEventHandler({EventHandlers::WindowManagerFullscreen<CDRenderer>,
                                   nullptr, renderer});
//    renderer->installEventHandler({EventHandlers::ExitOnQuitSignal<CDRenderer>,
//                                   nullptr, renderer});
    renderer->installEventHandler({StandardInput::StandardCamera<CGCamera>,
                                   nullptr, &loop->data->g_data.camera});

    loop->data->rt_queue = RuntimeQueue::CreateNewQueue("MainQueue");

    RuntimeQueue::Queue({
                            [](){
                                cDebug("Hello from RQ!");
                            },
                            {},
                            std::chrono::seconds(1),
                            RuntimeTask::Periodic,
                            0,
                        });

    CString err;
    if(CDRenderer::execEventLoop(*loop, props, err) != 0)
    {
        cWarning("Failed to start: {0}", err);
    }

    Profiler::PopContext();

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
