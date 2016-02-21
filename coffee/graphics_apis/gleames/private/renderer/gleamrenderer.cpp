#include <renderer/gleamrenderer.h>
#include <coffee_strings.h>
#include <glad/glad.h>
#include <SDL2/SDL_video.h>

namespace Coffee{
namespace CDisplay{

using GL = CGL::CGL_Implementation;

void gleamcallback(GLenum src, GLenum type,GLuint id,GLenum sev,GLsizei,const GLchar* msg,
                   const void* param)
{
    const CGLeamRenderer* renderer = (const CGLeamRenderer*)param;
    CGL::CGL_Implementation::CGDbgMsg cmsg;
//    cmsg.comp = CGL::CGL_Implementation::to_enum(src);
//    cmsg.type = CGL::CGL_Implementation::to_enum(type);
//    cmsg.sev = CGL::CGL_Implementation::to_enum(sev);
    cmsg.id = id;
    cmsg.msg = msg;
    renderer->bindingCallback(&cmsg);
}

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

    const static CGLVersion v30(3,0);

    bool status = false;

    if(m_properties.gl.version==v30)
        status = CGL::CGLES30::LoadBinding(this->glContext(),SDL_GL_GetProcAddress);

    if(!status)
    {
        cLog(__FILE__,__LINE__,CFStrings::Graphics_GLeam_Renderer_Name,
             CFStrings::Graphics_GLeam_Renderer_FailLoad);
        this->popErrorMessage(Severity::Fatal,
                              CFStrings::Graphics_GLeam_Renderer_Name,
                              CFStrings::Graphics_GLeam_Renderer_FailLoad);
        /*Context or graphics card on fire? Just get out!*/
        throw std::runtime_error(CFStrings::Graphics_GLeam_Renderer_Name);
    }
    if(GL::DebuggingSupported())
        GL::Debug::DebugSetCallback(gleamcallback,this);
}

void CGLeamRenderer::bindingTerminate()
{
}

CGLeamRenderer::~CGLeamRenderer()
{
}

void CGLeamRenderer::bindingCallback(const void *report) const
{
    const CGL::CGL_Implementation::CGDbgMsg* msg =
            (const CGL::CGL_Implementation::CGDbgMsg*)report;
    cBasicPrint("GLES:{0}:{1}:{2}:{3}: {4}",
                msg->comp,msg->sev,msg->type,
                msg->id,msg->msg.c_str());
}

}
}
