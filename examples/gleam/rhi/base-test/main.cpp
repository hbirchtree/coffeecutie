#include "renderer.h"
#include <coffee/core/CApplication>
#include <coffee/core/coffee.h>
#include <coffee/core/input/eventhandlers.h>
#include <coffee/core/input/standard_input_handlers.h>
#include <coffee/core/task_queue/task.h>

#if defined(FEATURE_ENABLE_CoffeeASIO)
#include <coffee/asio/net_profiling.h>
#endif

using EDATA = EventLoopData<CDRenderer, RendererState>;

int32 coffee_main(int32, cstring_w*)
{
    using namespace EventHandlers;
    using namespace StandardInput;

#if defined(FEATURE_ENABLE_CoffeeASIO)
    Net::RegisterProfiling();
#endif

    CDProperties props = GetDefaultVisual<RHI::GLEAM::GLEAM_API>();
    EDATA*       loop  = new EDATA{CreateRendererUq(),
                            MkUq<RendererState>(),
                            SetupRendering,
                            RendererLoop,
                            RendererCleanup};

    auto renderer = loop->renderer.get();
    /* Install some standard event handlers */
    renderer->installEventHandler(
        {EscapeCloseWindow<CDRenderer>, nullptr, renderer});
    renderer->installEventHandler(
        {WindowManagerCloseWindow<CDRenderer>, nullptr, renderer});
    renderer->installEventHandler(
        {ResizeWindowUniversal<GLM>, nullptr, renderer});
    renderer->installEventHandler(
        {WindowManagerFullscreen<CDRenderer>, nullptr, renderer});
    renderer->installEventHandler(
        {StandardCamera<CGCamera>, nullptr, loop->d()->camera_cnt.get(0)});

    runtime_queue_error ec;
    loop->data->rt_queue = RuntimeQueue::GetCurrentQueue(ec);

    C_ERROR_CHECK(ec);

    RuntimeQueue::Queue(
        {[loop]() { loop->d()->entities.exec(); },
         {},
         std::chrono::milliseconds(10),
         RuntimeTask::Periodic,
         0},
        ec);

    CString err;
    if(CDRenderer::execEventLoop(*loop, props, err) != 0)
        cWarning("Failed to start: {0}", err);

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
