#include <coffee/graphics_apis/gleam/renderer/gleamrenderer.h>
#include <coffee/core/coffee_strings.h>

#include <coffee/graphics_apis/gleam/gleam.h>

#ifdef COFFEE_GLEAM_DESKTOP
#include <coffee/graphics_apis/gleam/levels/desktop/gl45.h>
#else
#include <coffee/graphics_apis/gleam/levels/es/gles30.h>
#endif

#ifdef COFFEE_ANDROID
#include <SDL2/SDL_video.h>
#endif

#include "conversion.h"

namespace Coffee{
namespace CDisplay{

using GL = CGL::CGL_Implementation;

void gleamcallback(GLenum src, GLenum type,GLuint id,GLenum sev,GLsizei,const GLchar* msg,
                   const void* param)
{
    const CGLeamRenderer* renderer = (const CGLeamRenderer*)param;
    CGL::CGL_Implementation::CGDbgMsg cmsg;
#ifdef COFFEE_GLEAM_DESKTOP
    cmsg.sev = gl_convertsev(sev);
    cmsg.type = gl_converttype(type);
    cmsg.comp = gl_convertcomp(src);
#endif
    cmsg.id = id;
    cmsg.msg = msg;
    renderer->bindingCallback(&cmsg);
}

CGLeamRenderer::CGLeamRenderer(CObject* parent):
    CSDL2Renderer(parent)
{
}

void CGLeamRenderer::bindingPreInit(const GLProperties&)
{
}

void CGLeamRenderer::bindingInit(const GLProperties&)
{
}

void CGLeamRenderer::bindingPostInit(const GLProperties& p)
{
    Profiler::PushContext("GLeam");

    this->glContext()->acquireContext();

    Profiler::Profile("Context acquisition");

    bool status = false;

#ifdef COFFEE_GLEAM_DESKTOP
    const static CGLVersion v33(3,3);
    const static CGLVersion v43(4,3);
    const static CGLVersion v45(4,5);

    if(p.version>=v45)
        status = CGL::CGL45::LoadBinding(this->glContext());
    if(p.version>=v43)
        status = CGL::CGL43::LoadBinding(this->glContext());
    if(p.version>=v33)
        status = CGL::CGL33::LoadBinding(this->glContext());
#else
    const static CGLVersion v30es(3,0);

    if(p.version==v30es)
        status = CGL::CGLES30::LoadBinding(this->glContext(),SDL_GL_GetProcAddress);
#endif

    Profiler::Profile("Loading GL function pointers");

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
#ifdef COFFEE_GLEAM_DESKTOP
    if(GL::DebuggingSupported())
    {
#ifndef COFFEE_WINDOWS
        GL::Debug::SetDebugMode(true);
        GL::Debug::DebugSetCallback(gleamcallback,this);
#endif
    }
#endif

    Profiler::Profile("Debug setup");

    Profiler::PopContext();
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
    cBasicPrint("GL:{0}:{1}:{2}:{3}: {4}",
                msg->comp,msg->sev,msg->type,
                msg->id,msg->msg.c_str());
}

}
}
