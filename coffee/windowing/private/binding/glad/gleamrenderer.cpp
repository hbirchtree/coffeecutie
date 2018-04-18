#include <coffee/windowing/binding/glad/gleamrenderer.h>

#include <coffee/core/coffee_strings.h>
#include <coffee/core/platform_data.h>

#include <coffee/graphics/apis/gleam/gleam.h>
#include <coffee/core/CProfiling>

#if defined(COFFEE_USE_APPLE_GLKIT)
#include <CEAGL/eagl.h>
#elif defined(COFFEE_USE_MAEMO_EGL)
#include <EGL/egl.h>
#elif defined(COFFEE_USE_LINUX_GLX)
#define __gl_h_
#include <GL/glx.h>
#else
#include <SDL_video.h>
#endif

#include "conversion.h"

#define GLR_API "GLeamRenderer::"

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
void gleamcallback(
        GLenum src, GLenum type,GLuint id,GLenum sev,
        GLsizei,const GLchar* msg,
        const void* param
        )
{
    if(src == GL_DEBUG_SOURCE_APPLICATION)
        return;

    const GLeamRenderer* renderer = (const GLeamRenderer*)param;
    CGL::CGDbgMsg cmsg;


    cmsg.sev = gl_convertsev(sev);
    cmsg.type = gl_converttype(type);
    cmsg.comp = gl_convertcomp(src);
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
    (void)msg;
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
    DProfContext a(GLR_API "Binding initialization");

//    cVerbose(8, "Acquiring GL context from {0}, {1}",
//             (u64)m_app, (u64)m_app->glContext());

    if(!m_app->glContext()->acquireContext())
    {
        Profiler::DeepProfile(GLR_API "Failed to acquire GL context");
        cWarning(GLR_API "Failed to acquire GL context");
        return false;
    }

    bool status = false;

    cVerbose(8, GLR_API "Attempting to load version: {0}",p.version);

#if !defined(COFFEE_GLEAM_DESKTOP) || defined(COFFEE_USE_MAEMO_EGL)

#if !defined(COFFEE_LINKED_GLES)
#if defined(COFFEE_USE_LINUX_GLX)
    GLADloadproc procload = (GLADloadproc)glXGetProcAddress;
#elif defined(COFFEE_USE_MAEMO_EGL)
    GLADloadproc procload = (GLADloadproc)eglGetProcAddress;
#else
    GLADloadproc procload = SDL_GL_GetProcAddress;
#endif
#endif

#else
    GLADloadproc procload = nullptr;
#endif

    Profiler::DeepPushContext(GLR_API "Loading GLAD binding");
    if(!(p.flags & GLProperties::Flags::GLES))
    {
#ifdef COFFEE_GLEAM_DESKTOP
        const static CGLVersion v33(3,3);
        const static CGLVersion v43(4,3);
        const static CGLVersion v45(4,5);
        const static CGLVersion v46(4,6);

        if(p.version>=v45)
        {
            status = CGL::CGL45::LoadBinding(m_app->glContext(), procload);
        }else if(p.version>=v43)
        {
            status = CGL::CGL43::LoadBinding(m_app->glContext(), procload);
        } else if(p.version>=v33)
        {
            status = CGL::CGL33::LoadBinding(m_app->glContext(), procload);
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
            status = CGL::CGLES32::LoadBinding(m_app->glContext(),procload);
        }else
        if(p.version==v30es)
        {
            status = CGL::CGLES30::LoadBinding(m_app->glContext(),procload);
        }else
#endif
        if(p.version==v20es)
        {
#if !defined(COFFEE_LINKED_GLES)
            status = CGL::CGLES20::LoadBinding(m_app->glContext(),procload);
#else
            cVerbose(7, "Checking OpenGL ES 2.0 functions...");
            status = CGL::CGLES20::LoadBinding(m_app->glContext(),nullptr);
#endif
        }
#endif
    }
    Profiler::DeepPopContext();

    Profiler::DeepProfile(GLR_API "Loading GL function pointers");
    cVerbose(7, GLR_API "Function pointer checks succeeded: {0}", status);

    if(!status)
    {
//        cLog(__FILE__,__LINE__,CFStrings::Graphics_GLeam_Renderer_Name,
//             CFStrings::Graphics_GLeam_Renderer_FailLoad);
        /*Context or graphics card on fire? Just get out!*/
        if(err)
            *err = CFStrings::Graphics_GLeam_Renderer_FailLoad;
        Profiler::DeepProfile(GLR_API "glad failed to load");
        return false;
    }

    if(PlatformData::IsDebug())
    {
        GL::Debug::SetDebugGroup(GLR_API "Print information");

        Profiler::DeepPushContext(GLR_API "Printing information about GL");
        cDebug(GLR_API "Rendering device info: {0}",GL::Debug::Renderer());

        if(feval(p.flags&GLProperties::GLCoreProfile))
            cDebug(GLR_API "OpenGL core profile version: {0}",
                   GL::Debug::ContextVersion());
        else
            cDebug(GLR_API "OpenGL (non-core) version: {0}",
                   GL::Debug::ContextVersion());

        GL::Debug::UnsetDebugGroup();
    }

    cDebug(GLR_API "OpenGL GLSL version: {0}",
           GL::Debug::ShaderLanguageVersion());
    Profiler::DeepPopContext();

    if(PlatformData::IsDebug())
    {
        GL::Debug::SetDebugGroup(GLR_API "Profiler statistics");

        DProfContext b(GLR_API "Adding GL information to profiler");

        Profiler::AddExtraData(
                    "gl:renderer",
                    Strings::to_string(GL::Debug::Renderer()));
        Profiler::AddExtraData(
                    "gl:version",
                    Strings::to_string(GL::Debug::ContextVersion()));
        Profiler::AddExtraData(
                    "gl:glsl_version",
                    Strings::to_string(
                        GL::Debug::ShaderLanguageVersion()));
        Profiler::AddExtraData(
                    "gl:extensions",
                    GL::Debug::s_ExtensionList);
        Profiler::AddExtraData(
                    "gl:driver",
                    GL::Debug::ContextVersion().driver);

        GL::Debug::InitCompressedFormats();

        CString internalFormats = {};
        for(szptr i : Range<>(C_FCAST<szptr>(
                                  GL::Debug::Num_Internal_Formats)))
        {
            if(GL::Debug::Internal_Formats[i] > 0)

            internalFormats.append(
                        StrUtil::pointerify(
                            C_FCAST<u64>(
                                GL::Debug::Internal_Formats[i])&0xFFFF
                            )
                        + " ");
        }

        internalFormats = StrUtil::rtrim(internalFormats);

        Profiler::AddExtraData("gl:texformats", internalFormats);

        /* TODO: Add GL limits to extra data */

        using LIM = CGL::CGL_Shared_Limits;
        CString limits = {};
        for(u32 i=50; i<351; i+=50)
            for(u32 j=0; j<LIM::Max_Shader_Property; j++)
            {
                u32 val = i + j;
                i32 limit = LIM::Max(val);

                if(limit == 0)
                    continue;

                auto name = LIM::MaxName(val);
                CString label = {};
                if(name)
                    label = name;
                else
                    label = cast_pod(i);

                limits += label + "="
                        + cast_pod(limit)
                        + ",";
            }

        for(u32 i=LIM::Vertex_Attribs; i<LIM::Max_Property; i++)
        {
            i32 limit = LIM::Max(i);

            if(limit == 0)
                continue;

            auto name = LIM::MaxName(i);
            CString label = {};
            if(name)
                label = name;
            else
                label = cast_pod(i);

            limits += label + "="
                    + cast_pod(limit)
                    + ",";
        }

        Profiler::AddExtraData("gl:limits", limits);

        GL::Debug::UnsetDebugGroup();
    }

    if(GL::DebuggingSupported())
    {
#if !defined(COFFEE_WINDOWS) && !defined(COFFEE_ONLY_GLES20)
        DProfContext b(GLR_API "Configuring GL context debugging");
        GL::Debug::SetDebugMode(true);
        GL::Debug::DebugSetCallback(gleamcallback,this);
        GL::Debug::SetDebugLevel(Severity::Information, false);
#endif
    }


    return true;
}

void GLeamRenderer::bindingTerminate()
{
    GL::Debug::FreeInternalFormats();
}

}
}
