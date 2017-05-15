#include <coffee/core/CApplication>
#include <coffee/sdl2/CSDL2Dialog>
#include <coffee/core/input/eventhandlers.h>
#include <coffee/core/coffee.h>
#include "renderer.h"

void ExitOnBackground(void* user_ptr, CDEvent const& ev, c_cptr data)
{
    auto r = C_CAST<CDRenderer*>(user_ptr);

    cDebug("Caught window event: {0}", C_CAST<uint32>(ev.type));

    if(ev.type == CDEvent::Focus)
    {
        auto state = C_CAST<CDFocusEvent const*>(data);
        if(state->mod == CDFocusEvent::Leave)
            exit(0);
    }
}

int32 coffee_main(int32, cstring_w*)
{
    /* Set a prefix from which resources are fetched */
    CResources::FileResourcePrefix("sample_data/eye-demo/");
    SetPrintingVerbosity(8);
    GotoApplicationDir();

    /* Required for SDL2 applications, initializes SDL state */
    SubsystemWrapper<SDL2::SDL2> sdl2;
    C_UNUSED(sdl2);

    /*Moving on to regular rendering*/
    Profiler::PushContext("Root");

    CDRenderer actual_renderer;

    cDebug("Renderer object size: {0}", sizeof(actual_renderer));

    /* We use a reference for the virtual functions to work correctly */
    CSDL2Renderer* renderer = &actual_renderer;

    /* Install some standard event handlers */
    renderer->installEventHandler({EventHandlers::EscapeCloseWindow<CDRenderer>,
                                   nullptr,renderer});
    renderer->installEventHandler({EventHandlers::WindowManagerCloseWindow<CDRenderer>,
                                   nullptr,renderer});
    renderer->installEventHandler({EventHandlers::ResizeWindowUniversal<CDRenderer::GLM>,
                                   nullptr,renderer});
    renderer->installEventHandler({EventHandlers::WindowManagerFullscreen<CDRenderer>,
                                  nullptr,renderer});
    renderer->installEventHandler({EventHandlers::StandardCamera<CGCamera>,
                                  nullptr, &actual_renderer.cameraReference()});

//    renderer->installEventHandler({ExitOnBackground, nullptr, renderer});

    Profiler::Profile("Object creation");


    /* Set up the window visual */
    CDProperties props = GetDefaultVisual<RHI::GLEAM::GLEAM_API>();

    cDebug("Property object size: {0}", sizeof(props));

    props.flags ^= CDProperties::Resizable;
    props.gl.flags |= GLProperties::GLDebug;

    CString err;

    /* Initialize the window and check for errors */
    if(!LoadHighestVersion(renderer,props,&err))
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

    Profiler::Profile("Cleanup");

    Profiler::PopContext();

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
