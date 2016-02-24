#include <coffee/sdl2/csdl2renderer.h>

#include <coffee/core/CDebug>
#include <coffee/core/base/files/cfiles.h>

namespace Coffee{
namespace CDisplay{

using namespace Coffee::CSDL2Types;

CSDL2Renderer::CSDL2Renderer(CObject *parent) :
    CDRendererBase(parent)
{
}

CSDL2Renderer::~CSDL2Renderer()
{
    cleanup();
}

void CSDL2Renderer::init(const CDProperties &props)
{
    Profiler::PushContext("SDL2");

    m_properties = props;

    windowPreInit(initialProperties());
    contextPreInit(initialProperties().gl);
    bindingPreInit(initialProperties().gl);
    inputPreInit();

    windowInit(initialProperties());
    contextInit(initialProperties().gl);
    bindingInit(initialProperties().gl);
    inputInit();

    /* Run binding post-init, fetches GL extensions and etc. */
    windowPostInit(initialProperties());
    contextPostInit(initialProperties().gl);
    bindingPostInit(initialProperties().gl);
    inputPostInit();

    Profiler::PopContext();
}

void CSDL2Renderer::cleanup()
{
    if(getSDL2Context()){
        inputTerminate();
        contextTerminate();
        windowTerminate();
    }else{
        /* This happens if cleanup has happened before destruction, or if cleanup is called multiple times. Either way is fine. */
        cMsg("SDL2","Already cleaned up");
    }
}

}
}
