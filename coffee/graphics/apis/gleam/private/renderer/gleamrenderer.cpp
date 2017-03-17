#include <coffee/graphics/apis/gleam/renderer/gleamrenderer.h>
#include <coffee/core/coffee_strings.h>
#include <coffee/core/platform_data.h>

#include <coffee/graphics/apis/gleam/gleam.h>
#include <coffee/core/CProfiling>

#ifndef COFFEE_GLEAM_DESKTOP
#include <SDL_video.h>
#endif

#include "conversion.h"

namespace Coffee{
namespace Display{

using GL = CGL::CGL_Implementation;

GLeamRenderer::GLeamRenderer(GLApplication *app):
    GLLoader(app)
{
}

GLeamRenderer::~GLeamRenderer()
{
}

#if !defined(COFFEE_ONLY_GLES20)
void gleamcallback(GLenum src, GLenum type,GLuint id,GLenum sev,GLsizei,const GLchar* msg,
                   const void* param)
{
    const GLeamRenderer* renderer = (const GLeamRenderer*)param;
    CGL::CGDbgMsg cmsg;
#ifdef COFFEE_GLEAM_DESKTOP
    cmsg.sev = gl_convertsev(sev);
    cmsg.type = gl_converttype(type);
    cmsg.comp = gl_convertcomp(src);
#endif
    cmsg.id = id;
    cmsg.msg = msg;
    renderer->bindingCallback(&cmsg);
}
#endif

void GLeamRenderer::bindingCallback(const void *report) const
{
    const CGL::CGDbgMsg* msg =
            (const CGL::CGDbgMsg*)report;
    cBasicPrint("GL:{0}:{1}:{2}:{3}: {4}",
                msg->comp,msg->sev,msg->type,
                msg->id,msg->msg.c_str());
}

bool GLeamRenderer::bindingPreInit(const GLProperties&,CString*)
{
    return true;
}

bool GLeamRenderer::bindingInit(const GLProperties&,CString*)
{
    return true;
}

bool GLeamRenderer::bindingPostInit(const GLProperties& p, CString *err)
{
    Profiler::PushContext("GLeam");

    m_app->glContext()->acquireContext();

    Profiler::Profile("Context acquisition");

    bool status = false;

    cDebug("Attempting to load version: {0}",p.version);

    if(!(p.flags & GLProperties::Flags::GLES))
    {
#ifdef COFFEE_GLEAM_DESKTOP
        const static CGLVersion v33(3,3);
        const static CGLVersion v43(4,3);
        const static CGLVersion v45(4,5);

        if(p.version>=v45)
        {
//            cDebug("Loading context version: GL {0}",(_cbasic_version<uint8> const&)v45);
            status = CGL::CGL45::LoadBinding(m_app->glContext());
        }else if(p.version>=v43)
        {
//            cDebug("Loading context version: GL {0}",(_cbasic_version<uint8> const&)v43);
            status = CGL::CGL43::LoadBinding(m_app->glContext());
        } else if(p.version>=v33)
        {
//            cDebug("Loading context version: GL {0}",(_cbasic_version<uint8> const&)v33);
            status = CGL::CGL33::LoadBinding(m_app->glContext());
        }
#endif
    }else{
#ifndef COFFEE_GLEAM_DESKTOP
        const static CGLVersion v20es(2,0);
#if !defined(COFFEE_ONLY_GLES20)
        const static CGLVersion v30es(3,0);
        const static CGLVersion v32es(3,2);
#endif

#if !defined(COFFEE_ONLY_GLES20)
        if(p.version>=v32es)
        {
            status = CGL::CGLES32::LoadBinding(m_app->glContext(),SDL_GL_GetProcAddress);
        }else
        if(p.version==v30es)
        {
            status = CGL::CGLES30::LoadBinding(m_app->glContext(),SDL_GL_GetProcAddress);
        }else
#endif
        if(p.version==v20es)
        {
#if !defined(COFFEE_LINKED_GLES)
            status = CGL::CGLES20::LoadBinding(m_app->glContext(),SDL_GL_GetProcAddress);
#else
            status = CGL::CGLES20::LoadBinding(m_app->glContext(),nullptr);
#endif
        }
#endif
    }

    Profiler::Profile("Loading GL function pointers");

    if(!status)
    {
//        cLog(__FILE__,__LINE__,CFStrings::Graphics_GLeam_Renderer_Name,
//             CFStrings::Graphics_GLeam_Renderer_FailLoad);
        /*Context or graphics card on fire? Just get out!*/
        if(err)
            *err = CFStrings::Graphics_GLeam_Renderer_FailLoad;
        return false;
    }

    cDebug("Rendering device info: {0}",GL::Debug::Renderer());
    if(feval(p.flags&GLProperties::GLCoreProfile))
        cDebug("OpenGL core profile version: {0}",GL::Debug::ContextVersion());
    else
        cDebug("OpenGL (non-core) version: {0}",GL::Debug::ContextVersion());
    cDebug("OpenGL GLSL version: {0}",GL::Debug::ShaderLanguageVersion());

    if(GL::DebuggingSupported())
    {
#if !defined(COFFEE_WINDOWS) && !defined(COFFEE_ONLY_GLES20)
        GL::Debug::SetDebugMode(true);
        GL::Debug::DebugSetCallback(gleamcallback,this);
#endif
    }

    Profiler::Profile("Debug setup");
    return true;
}

void GLeamRenderer::bindingTerminate()
{
    GL::Debug::FreeInternalFormats();
}

}
}
