#include "renderer_internal.h"

#include <coffee/core/CDebug>
#include <coffee/core/base/files/cfiles.h>

#if defined(COFFEE_USE_SDL_EVENT) || defined(COFFEE_USE_SDL_GL) || defined(COFFEE_USE_SDL_WINDOW)
#include <coffee/sdl2/sdl2system.h>
#endif

#define RENDERER_TAG "RendererInterface::"

namespace Coffee{
namespace Display{

using namespace Coffee::SDL2;

CSDL2Renderer_Internal::CSDL2Renderer_Internal(CObject *parent) :
    GLLoader(this)
  #if defined(COFFEE_ENABLE_OPENGL)
    , GLeamRenderer(this)
  #endif
{
}

CSDL2Renderer_Internal::CSDL2Renderer_Internal():
    CSDL2Renderer_Internal(nullptr)
{
}

CSDL2Renderer_Internal::~CSDL2Renderer_Internal()
{
    cleanup();
}

bool CSDL2Renderer_Internal::init(const CDProperties &props,CString* err)
{
#if defined(COFFEE_USE_SDL_EVENT) || defined(COFFEE_USE_SDL_GL) || defined(COFFEE_USE_SDL_WINDOW)
    m_properties = props;
#endif

    DProfContext a(RENDERER_TAG "Initializing renderer");

    do{
        if(!(windowPreInit(props,err)
     #if defined(COFFEE_ENABLE_OPENGL)
             && contextPreInit(props.gl,err)
             && bindingPreInit(props.gl,err)
     #endif
             && inputPreInit(err)
             ))
            break;

        if(!(windowInit(props,err)
     #if defined(COFFEE_ENABLE_OPENGL)
             && contextInit(props.gl,err)
             && bindingInit(props.gl,err)
     #endif
             && inputInit(err)
             ))
            break;

        /* Run binding post-init, fetches GL extensions and etc. */
        if(!(windowPostInit(props,err)
     #if defined(COFFEE_ENABLE_OPENGL)
             && contextPostInit(props.gl,err)
             && bindingPostInit(props.gl,err)
     #endif
             && inputPostInit(err)
             ))
            break;

        ExtraData::Add("window:library", windowLibrary());

        return true;
    } while(false);

    cleanup();

    Profiler::DeepProfile("Failed to initialize renderer");

    return false;
}

void CSDL2Renderer_Internal::cleanup()
{
    DProfContext a(RENDERER_TAG "Cleaning up renderer");

#if defined(COFFEE_ENABLE_OPENGL)
    bindingTerminate();
#if !defined(COFFEE_USE_SDL_GL)
    contextTerminate();
#endif
#endif

#if !defined(COFFEE_USE_SDL_EVENT)
    inputTerminate();
#endif
#if !defined(COFFEE_USE_SDL_WINDOW)
    windowTerminate();
#endif

#if defined(COFFEE_USE_SDL_EVENT) || defined(COFFEE_USE_SDL_GL) || defined(COFFEE_USE_SDL_WINDOW)
    if(getSDL2Context()){
#if defined(COFFEE_USE_SDL_GL)
        contextTerminate();
#endif
#if defined(COFFEE_USE_SDL_EVENT)
        inputTerminate();
#endif
#if defined(COFFEE_USE_SDL_WINDOW)
        windowTerminate();

        Coffee::SDL2::SDL2::Deinit();
#endif
    }else{
        /* This happens if cleanup has happened before destruction,
         *  or if cleanup is called multiple times. Either way is fine. */
        cMsg("SDL2","Already cleaned up");
    }
#endif

}

void CSDL2Renderer_Internal::run()
{
}

}
}
