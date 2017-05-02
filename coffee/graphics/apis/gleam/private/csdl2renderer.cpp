#include <coffee/graphics/apis/gleam/csdl2renderer.h>

#include <coffee/core/CDebug>
#include <coffee/core/base/files/cfiles.h>

namespace Coffee{
namespace Display{

using namespace Coffee::SDL2;

CSDL2Renderer::CSDL2Renderer(CObject *parent) :
    CDRendererBase(parent),
    GLeamRenderer(this)
{
}

CSDL2Renderer::~CSDL2Renderer()
{
    cleanup();
}

bool CSDL2Renderer::init(const CDProperties &props,CString* err)
{
#if !defined(COFFEE_RASPBERRY_DMX)
    m_properties = props;
#endif

    do{
        if(!(windowPreInit(props,err)
             && contextPreInit(props.gl,err)
             && bindingPreInit(props.gl,err)
             && inputPreInit(err)
             ))
            break;

        if(!(windowInit(props,err)
             && contextInit(props.gl,err)
             && bindingInit(props.gl,err)
             && inputInit(err)
             ))
            break;

        /* Run binding post-init, fetches GL extensions and etc. */
        if(!(windowPostInit(props,err)
             && contextPostInit(props.gl,err)
             && bindingPostInit(props.gl,err)
             && inputPostInit(err)
             ))
            break;

        Profiler::AddExtraData("window_library", windowLibrary());

        return true;
    } while(false);

    cleanup();

    return false;
}

#if defined(COFFEE_RASPBERRY_DMX)
#define DISABLE_SDL2
#endif

#if defined(COFFEE_USE_MAEMO_EGL) || defined(DISABLE_SDL2)
#define USE_SEPARATE_CONTEXT_MGR
#endif

#if defined(COFFEE_USE_MAEMO_X11) || defined(DISABLE_SDL2)
#define USE_SEPARATE_WINDOW_MGR
#endif

#if defined(DISABLE_SDL2)
#define USE_SEPARATE_EVENT_MGR
#endif

void CSDL2Renderer::cleanup()
{
    bindingTerminate();
#if defined(USE_SEPARATE_CONTEXT_MGR)
    contextTerminate();
#endif
#if defined(USE_SEPARATE_EVENT_MGR)
    inputTerminate();
#endif
#if defined(USE_SEPARATE_WINDOW_MGR)
    windowTerminate();
#endif

#if !defined(DISABLE_SDL2)
    if(getSDL2Context()){
#if !defined(USE_SEPARATE_CONTEXT_MGR)
        contextTerminate();
#endif
        inputTerminate();
#if !defined(USE_SEPARATE_WINDOW_MGR)
        windowTerminate();
#endif
    }else{
        /* This happens if cleanup has happened before destruction, or if cleanup is called multiple times. Either way is fine. */
        cMsg("SDL2","Already cleaned up");
    }
#endif
}

void CSDL2Renderer::run()
{
}

}
}
