#include <renderer/gleamrenderer.h>
#include <coffee_strings.h>
#include <levels/gles30.h>
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
    switch(src)
    {
    case GL_DEBUG_SOURCE_API:
        cmsg.comp = DebugComponent::GraphicsAPI;
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        cmsg.comp = DebugComponent::Core;
        break;
    case GL_DEBUG_SOURCE_OTHER:
        cmsg.comp = DebugComponent::Interface;
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        cmsg.comp = DebugComponent::ShaderCompiler;
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        cmsg.comp = DebugComponent::GraphicsAPI;
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        cmsg.comp = DebugComponent::GraphicsAPI;
        break;
    default:
        cmsg.comp = DebugComponent::GraphicsAPI;
        break;
    }
    switch(type)
    {
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        cmsg.type = DebugType::Deprecated;
        break;
    case GL_DEBUG_TYPE_ERROR:
        cmsg.type = DebugType::UndefinedBehavior;
        break;
    case GL_DEBUG_TYPE_MARKER:
        cmsg.type = DebugType::Marker;
        break;
    case GL_DEBUG_TYPE_OTHER:
        cmsg.type = DebugType::UndefinedBehavior;
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        cmsg.type = DebugType::Performance;
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        cmsg.type = DebugType::Compliance;
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        cmsg.type = DebugType::UndefinedBehavior;
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        cmsg.type = DebugType::Information;
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        cmsg.type = DebugType::Information;
        break;
    default:
        cmsg.type = DebugType::Information;
        break;
    }
    switch(sev)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        cmsg.sev = Severity::High;
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        cmsg.sev = Severity::Medium;
        break;
    case GL_DEBUG_SEVERITY_LOW:
        cmsg.sev = Severity::Low;
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        cmsg.sev = Severity::Information;
        break;
    default:
        cmsg.sev = Severity::Information;
        break;
    }
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
