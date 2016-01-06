#include <renderer/gleamrenderer.h>
#include <coffee/core/coffee_strings.h>
#include <levels/gl33.h>
#include <glad/glad.h>

namespace Coffee{
namespace CDisplay{

CGLeamRenderer::CGLeamRenderer(CObject* parent):
    CSDL2Renderer(parent)
{
}

void CGLeamRenderer::bindingPreInit()
{
}

void CGLeamRenderer::bindingPostInit()
{
    this->glContext()->acquireContext();
    if(!CGL::CGL33::LoadBinding(this->glContext()))
    {
        cLog(__FILE__,__LINE__,CFStrings::Graphics_GLeam_Renderer_Name,
             CFStrings::Graphics_GLeam_Renderer_FailLoad);
        throw std::runtime_error(CFStrings::Graphics_GLeam_Renderer_Name);
    }
}

void CGLeamRenderer::bindingTerminate()
{
}

CGLeamRenderer::~CGLeamRenderer()
{
}

void CGLeamRenderer::bindingCallback(const void *report) const
{

    cBasicPrint("GL:{0}:{1}:{2}: {3}");
}

}
}
