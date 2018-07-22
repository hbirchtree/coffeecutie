#include <coffee/windowing/binding/glad/gleamrenderer.h>

#include <coffee/core/coffee_strings.h>
#include <coffee/core/platform_data.h>

#include <coffee/core/CDebug>
#include <coffee/core/CProfiling>
#include <coffee/graphics/apis/gleam/gleam.h>

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


namespace Coffee {
namespace Display {

#if defined(COFFEE_GLEAM_DESKTOP)
using GLDEBUG = CGL::CGL43;
#else
using GLDEBUG = CGL::CGLES32;
#endif

using GL = CGL::CGL_Lowest;
using GDEBUG = CGL::Debug;

static void PushDebugGroup(cstring n)
{
#if GL_VERSION_VERIFY(0x430, 0x320)
    if(glPushDebugGroup)
        GLDEBUG::PushDebugGroup(GL_DEBUG_SOURCE_APPLICATION_KHR, 0, -1, n);
#endif
}

static void PopDebugGroup()
{
#if GL_VERSION_VERIFY(0x430, 0x320)
    if(glPopDebugGroup)
        GLDEBUG::PopDebugGroup();
#endif
}

GLeamRenderer::GLeamRenderer(GLApplication* app) : GLLoader(app)
{
}

GLeamRenderer::~GLeamRenderer()
{
}

#if GL_VERSION_VERIFY(0x330, 0x320)
void gleamcallback(
    GLenum src,
    GLenum type,
    GLuint id,
    GLenum sev,
    GLsizei,
    const GLchar* msg,
    const void*   param)
{
//    if(src == GL_DEBUG_SOURCE_APPLICATION)
//        return;

    const GLeamRenderer* renderer = C_RCAST<const GLeamRenderer*>(param);
    CGL::CGDbgMsg        cmsg;

    cmsg.sev  = gl_convertsev(sev);
    cmsg.type = gl_converttype(type);
    cmsg.comp = gl_convertcomp(src);
    cmsg.id   = id;
    cmsg.msg  = msg;
    renderer->bindingCallback(&cmsg);
}
#endif

void GLeamRenderer::bindingCallback(const void* report) const
{
    const CGL::CGDbgMsg* msg = C_RCAST<const CGL::CGDbgMsg*>(report);
    cBasicPrint(
        "GL:{0}:{1}:{2}:{3}: {4}",
        msg->comp,
        msg->sev,
        msg->type,
        msg->id,
        msg->msg.c_str());
    (void)msg;

    C_BREAK();
}

bool GLeamRenderer::bindingPreInit(const GLProperties&, CString*)
{
    return true;
}

bool GLeamRenderer::bindingInit(const GLProperties&, CString*)
{
    return true;
}

bool GLeamRenderer::bindingPostInit(const GLProperties& p, CString* err)
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

    cVerbose(8, GLR_API "Attempting to load version: {0}", p.version);

    /* When rendering with GLES or EGL, we need a procloader most likely */
#if !defined(COFFEE_GLEAM_DESKTOP) || defined(COFFEE_USE_MAEMO_EGL)

    /* If GLES functions are linked, skip this */
#if !defined(COFFEE_LINKED_GLES)
    /* Otherwise, pick a procloader based on the current windowing API */
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
#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
        const static CGLVersion v33(3, 3);
        const static CGLVersion v43(4, 3);
        const static CGLVersion v45(4, 5);
        const static CGLVersion v46(4, 6);

        if(p.version >= v45)
        {
            status = CGL::Loader::LoadBinding(
                m_app->glContext(),
                procload,
                reinterpret_cast<void**>(&glTextureBarrier));
        } else if(p.version >= v43)
        {
            status = CGL::Loader::LoadBinding(
                m_app->glContext(),
                procload,
                reinterpret_cast<void**>(&glDispatchCompute));
        } else if(p.version >= v33)
        {
            status = CGL::Loader::LoadBinding(
                m_app->glContext(),
                procload,
                reinterpret_cast<void**>(&glCreateSamplers));
        }
#endif
    } else
    {
#if GL_VERSION_VERIFY(GL_VERSION_NONE, 0x200)
        const static CGLVersion v20es(2, 0);
#if GL_VERSION_VERIFY(GL_VERSION_NONE, 0x300)
        const static CGLVersion v30es(3, 0);
        const static CGLVersion v32es(3, 2);
#endif

#if !defined(COFFEE_LINKED_GLES)
#if GL_VERSION_VERIFY(0x0, 0x320)
        if(p.version >= v32es)
        {
            status = CGL::Loader::LoadBinding(
                m_app->glContext(),
                procload,
                reinterpret_cast<void**>(&glVertexAttribFormat));
        } else
#endif
#if GL_VERSION_VERIFY(0x0, 0x300)
            if(p.version == v30es)
        {
            status = CGL::Loader::LoadBinding(
                m_app->glContext(),
                procload,
                reinterpret_cast<void**>(&glTexStorage2D));
        } else
#endif
            if(p.version == v20es)
        {
            status = CGL::Loader::LoadBinding(
                m_app->glContext(),
                procload,
                reinterpret_cast<void**>(&glTexImage2D));
        }
#else
        status = true;
#endif
#endif
    }
    Profiler::DeepPopContext();

    Profiler::DeepProfile(GLR_API "Loading GL function pointers");
    cVerbose(7, GLR_API "Function pointer checks succeeded: {0}", status);

    if(!status)
    {
        if(err)
            *err = CFStrings::Graphics_GLeam_Renderer_FailLoad;
        Profiler::DeepProfile(GLR_API "glad failed to load");
        return false;
    }

    cDebug(
        GLR_API "OpenGL GLSL version: {0}", GDEBUG::ShaderLanguageVersion());
    Profiler::DeepPopContext();

    GDEBUG::GetExtensions();

    if(PlatformData::IsDebug())
    {
        PushDebugGroup(GLR_API "Profiler statistics");

        DProfContext b(GLR_API "Adding GL information to profiler");

        Profiler::AddExtraData(
            "gl:renderer", Strings::to_string(GDEBUG::Renderer()));
        Profiler::AddExtraData(
            "gl:version", Strings::to_string(GDEBUG::ContextVersion()));
        Profiler::AddExtraData(
            "gl:glsl_version",
            Strings::to_string(GDEBUG::ShaderLanguageVersion()));
        Profiler::AddExtraData("gl:extensions", GDEBUG::s_ExtensionList);
        Profiler::AddExtraData("gl:driver", GDEBUG::ContextVersion().driver);

        GDEBUG::InitCompressedFormats();

        CString internalFormats = {};
        for(szptr i : Range<>(C_FCAST<szptr>(GDEBUG::Num_Internal_Formats)))
        {
            if(GDEBUG::Internal_Formats[i] > 0)

                internalFormats.append(
                    StrUtil::pointerify(
                        C_FCAST<u64>(GDEBUG::Internal_Formats[i]) & 0xFFFF) +
                    " ");
        }

        internalFormats = StrUtil::rtrim(internalFormats);

        Profiler::AddExtraData("gl:texformats", internalFormats);

        /* TODO: Add GL limits to extra data */

        using LIM      = CGL::CGL_Shared_Limits;
        CString limits = {};
        for(u32 i = 50; i < 351; i += 50)
            for(u32 j = 0; j < LIM::Max_Shader_Property; j++)
            {
                u32 val   = i + j;
                i32 limit = LIM::Max(val);

                if(limit == 0)
                    continue;

                auto    name  = LIM::MaxName(val);
                CString label = {};
                if(name)
                    label = name;
                else
                    label = cast_pod(i);

                limits += label + "=" + cast_pod(limit) + ",";
            }

        for(u32 i = LIM::Vertex_Attribs; i < LIM::Max_Property; i++)
        {
            i32 limit = LIM::Max(i);

            if(limit == 0)
                continue;

            auto    name  = LIM::MaxName(i);
            CString label = {};
            if(name)
                label = name;
            else
                label = cast_pod(i);

            limits += label + "=" + cast_pod(limit) + ",";
        }

        Profiler::AddExtraData("gl:limits", limits);

        PopDebugGroup();
    }

#if !defined(COFFEE_WINDOWS) && GL_VERSION_VERIFY(0x330, 0x320)
    if(p.flags & GLProperties::GLDebug && glDebugMessageCallback)
    {
        DProfContext b(GLR_API "Configuring GL context debugging");
        GDEBUG::SetDebugMode(true);
        GDEBUG::DebugSetCallback(gleamcallback, this);
        GDEBUG::SetDebugLevel(Severity::Information, false);
    }
#endif

    return true;
}

void GLeamRenderer::bindingTerminate()
{
    GDEBUG::FreeInternalFormats();
}

} // namespace Display
} // namespace Coffee
