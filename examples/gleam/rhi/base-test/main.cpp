#include <coffee/core/CApplication>
#include <coffee/sdl2/CSDL2Dialog>
#include <coffee/core/input/eventhandlers.h>
#include "renderer.h"

int32 coffee_main(int32, cstring_w*)
{
    cVerbose("Device: {0}",SysInfo::DeviceName());

    /* Set a prefix from which resources are fetched */
    CResources::FileResourcePrefix("sample_data/eye-demo/");
    GotoApplicationDir();

    /* Required for SDL2 applications, initializes SDL state */
    SubsystemWrapper<SDL2::SDL2> sdl2;
    C_UNUSED(sdl2);

    /*Moving on to regular rendering*/
    Profiler::PushContext("Root");

    CSDL2Renderer *renderer = new CDRenderer();

    /* Install some standard event handlers */
    renderer->installEventHandler({EventHandlers::EscapeCloseWindow<CDRenderer>,
                                   nullptr,renderer});
    renderer->installEventHandler({EventHandlers::WindowManagerCloseWindow<CDRenderer>,
                                   nullptr,renderer});
    renderer->installEventHandler({EventHandlers::ResizeWindowUniversal<CDRenderer::GLM>,
                                   nullptr,renderer});

    Profiler::Profile("Object creation");

    /* Set up the window visual */
    CDProperties props = GetDefaultVisual();
    props.flags ^= CDProperties::Resizable;
    props.gl.flags |= GLProperties::GLDebug;
//    props.gl.flags |= GLProperties::GLVSync;

    CString err;

    /* Initialize the window and check for errors */
    if(!renderer->init(props,&err))
    {
        SDL2Dialog::ErrorMessage("Initialization error",err.c_str());
        cDebug("Initialization error: {0}",err);
        return 1;
    }
    Profiler::Profile("Initialize renderer");
    cVerbose("Initialized renderer");

    /* Run the program */
    renderer->run();

    Profiler::Profile("Runtime");

    /* Clean all resources */
    renderer->cleanup();
    delete renderer;

    Profiler::Profile("Cleanup");

    Profiler::PopContext();

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
