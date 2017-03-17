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
    m_properties = props;

    do{
        if(!(windowPreInit(initialProperties(),err) &&
             contextPreInit(initialProperties().gl,err) &&
             bindingPreInit(initialProperties().gl,err) &&
             inputPreInit(err)))
            break;

        if(!(windowInit(initialProperties(),err) &&
             contextInit(initialProperties().gl,err) &&
             bindingInit(initialProperties().gl,err) &&
             inputInit(err)))
            break;

        /* Run binding post-init, fetches GL extensions and etc. */
        if(!(windowPostInit(initialProperties(),err) &&
             contextPostInit(initialProperties().gl,err) &&
             bindingPostInit(initialProperties().gl,err) &&
             inputPostInit(err)))
            break;

        return true;
    } while(false);

    cleanup();

    return false;
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

void CSDL2Renderer::run()
{
}

}
}
