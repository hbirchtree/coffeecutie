#include <coffee/sdl2/csdl2renderer.h>

#include <coffee/core/CDebug>
#include <coffee/core/base/files/cfiles.h>

namespace Coffee{
namespace Display{

using namespace Coffee::CSDL2Types;

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
    m_properties = props;

    if(!(windowPreInit(initialProperties(),err) &&
         contextPreInit(initialProperties().gl,err) &&
         bindingPreInit(initialProperties().gl,err) &&
         inputPreInit(err)))
        return false;

    if(!(windowInit(initialProperties(),err) &&
         contextInit(initialProperties().gl,err) &&
         bindingInit(initialProperties().gl,err) &&
         inputInit(err)))
        return false;

    /* Run binding post-init, fetches GL extensions and etc. */
    if(!(windowPostInit(initialProperties(),err) &&
         contextPostInit(initialProperties().gl,err) &&
         bindingPostInit(initialProperties().gl,err) &&
         inputPostInit(err)))
        return false;

    return true;
}

void CSDL2Renderer::cleanup()
{
    if(getSDL2Context()){
        inputTerminate();
        bindingTerminate();
        contextTerminate();
        windowTerminate();
    }else{
        /* This happens if cleanup has happened before destruction, or if cleanup is called multiple times. Either way is fine. */
        cMsg("SDL2","Already cleaned up");
    }
}

}
}
