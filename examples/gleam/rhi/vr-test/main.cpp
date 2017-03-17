#include <coffee/core/CApplication>
#include <coffee/sdl2/CSDL2Dialog>
#include <coffee/core/input/eventhandlers.h>
#include <coffee/core/coffee.h>

#include "renderer.h"

int32 coffee_main(int32, cstring_w*)
{
    /* Set a prefix from which resources are fetched */
    CResources::FileResourcePrefix("sample_data/eye-demo/");
    GotoApplicationDir();

    /* Required for SDL2 applications, initializes SDL state */
    SubsystemWrapper<SDL2::SDL2> sdl2;
    C_UNUSED(sdl2);

    /*Moving on to regular rendering*/
    Profiler::PushContext("Root");

    CDRenderer actual_renderer;

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

    Profiler::Profile("Object creation");


    /* Set up the window visual */
    CDProperties props = GetDefaultVisual<RHI::GLEAM::GLEAM_API>();

    CString err;

    /* Initialize the window and check for errors */
    if(!LoadHighestVersion(renderer,props,&err))
    {
        SDL2Dialog::ErrorMessage("Initialization error",err.c_str());
        cDebug("Initialization error: {0}",err);
    }

    do {
        CResources::Resource icon("icon.png",
                                  ResourceAccess::SpecifyStorage
                                  |ResourceAccess::AssetFile);
        if(!CResources::FileMap(icon))
            break;
        do {
            CStbImageLib::CStbImage img;
            if(!CStbImageLib::LoadData(&img,&icon))
                break;

            Bitmap bmp(img.size.w,img.size.h,(CRGBA*)img.data);
            renderer->setWindowIcon(bmp);

            CStbImageLib::ImageFree(&img);
        }while(false);
        CResources::FileUnmap(icon);
    }while(false);

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
