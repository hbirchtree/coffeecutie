#include <coffee/windowing/renderer/renderer.h>

#include <coffee/core/CDebug>
#include <coffee/core/base/files/cfiles.h>

#if defined(COFFEE_USE_SDL_EVENT) || defined(COFFEE_USE_SDL_GL) || defined(COFFEE_USE_SDL_WINDOW)
#include <coffee/sdl2/sdl2system.h>
#endif

namespace Coffee{
namespace Display{

using namespace Coffee::SDL2;

CSDL2Renderer::CSDL2Renderer(CObject *parent) :
    CDRendererBase(parent),
    GLeamRenderer(this)
{
}

CSDL2Renderer::CSDL2Renderer():
    CSDL2Renderer(nullptr)
{
}

CSDL2Renderer::~CSDL2Renderer()
{
    cleanup();
}

bool CSDL2Renderer::init(const CDProperties &props,CString* err)
{
#if defined(COFFEE_USE_SDL_EVENT) || defined(COFFEE_USE_SDL_GL) || defined(COFFEE_USE_SDL_WINDOW)
    m_properties = props;
#endif

    cDebug("Initial loading");

    do{
        if(!(windowPreInit(props,err)
             && contextPreInit(props.gl,err)
             && bindingPreInit(props.gl,err)
             && inputPreInit(err)
             ))
            break;

        cDebug("Passed pre-init");

        if(!(windowInit(props,err)
             && contextInit(props.gl,err)
             && bindingInit(props.gl,err)
             && inputInit(err)
             ))
            break;

        cDebug("Passed init");

        /* Run binding post-init, fetches GL extensions and etc. */
        if(!(windowPostInit(props,err)
             && contextPostInit(props.gl,err)
             && bindingPostInit(props.gl,err)
             && inputPostInit(err)
             ))
            break;

        cDebug("Passed post-init");

        Profiler::AddExtraData("window:library", windowLibrary());

        return true;
    } while(false);

    cleanup();

    return false;
}

void CSDL2Renderer::cleanup()
{
    bindingTerminate();
#if !defined(COFFEE_USE_SDL_GL)
    contextTerminate();
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

void CSDL2Renderer::run()
{
}

}
}
