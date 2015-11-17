#include "cglbindingrenderer.h"
#include "coffee/core/graphics_apis/COpenGL"

#include <glbinding/ContextInfo.h>
#include <glbinding/Version.h>
#include <glbinding/callbacks.h>
#include <glbinding/Binding.h>
#include <glbinding/Meta.h>

#include "coffee/core/graphics/opengl_glbinding/glbinding.h"
#include "coffee/core/graphics/cgraphics_quirks.h"

using namespace gl;
using namespace Coffee::CDisplay;

namespace Coffee {
namespace CDisplay {

void glbindingCallbackDirect(GLenum source, GLenum type,GLuint id, GLenum severity,GLsizei length, const GLchar* msg,const void* userPtr);

void glbindingCallbackDirect(GLenum source, GLenum type,
                                             GLuint id, GLenum severity,
                                             GLsizei length, const GLchar* msg,
                                             const void* userPtr)
{
    const CGLBindingRenderer* renderer = static_cast<const CGLBindingRenderer*>(userPtr);
    CGLReport *report = (CGLReport*)c_alloc(sizeof(CGLReport));
    report->source = source;
    report->type = type;
    report->id = id;
    report->message = msg;
    report->severity = severity;
    renderer->bindingCallback(report);
    c_free(report);
}

CGLBindingRenderer::CGLBindingRenderer(Coffee::CObject *parent) :
    CSDL2Renderer(parent),
    m_msg_filter(nullptr),
    m_extensions(nullptr)
{
}

CGLBindingRenderer::~CGLBindingRenderer()
{
    if(m_extensions)
        c_free(m_extensions);
}

void CGLBindingRenderer::fetchGLExtensions()
{
    if(m_extensions)
        c_free(m_extensions);

    GLint exts = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS,&exts);

    CString tmp;

    for(GLint i=0;i<exts;i++){
        tmp.append((byte*)glGetStringi(GL_EXTENSIONS,i));
        if(i<exts-1)
            tmp.push_back('\n');
    }

    m_extensions = (cstring_w)c_alloc(tmp.size()+1);
    strcpy(m_extensions,tmp.c_str());
}

bool CGLBindingRenderer::requestGLExtension(cstring ext)
{
    if(!m_extensions){
        if(std::this_thread::get_id()==contextThread())
            fetchGLExtensions();
        else{
            //In case we are on the wrong thread, try queueing a function
            //Will cause a deadlock if functions are not being processed
            cDebug("Failed to acquire extensions: Wrong thread, attempting workaround");
            std::atomic_int* atom = new std::atomic_int();
            atom->store(0);

            queueFunction([=](){
                fetchGLExtensions();
                (*atom)++;
            });

            while(atom->load()<1)
                std::this_thread::sleep_for(std::chrono::microseconds(100));

            return requestGLExtension(ext);
        }
    }
    return strstr(m_extensions,ext);
}

bool CGLBindingRenderer::printExtensions(bool doFetch)
{
    if(!m_extensions){
        if(doFetch)
            fetchGLExtensions();
        else
            return false;
    }

    cDebug("Now printing available extensions");
    cBasicPrint("-----------| Extensions |-----------");
    for(GLextension ext : glbinding::Meta::extensions()){
        cstring extname = glbinding::Meta::getString(ext).c_str();
        bool support = requestGLExtension(extname);
        if(support)
            cBasicPrint("Extension: %s, core version: %s",
                        extname,
                        glbinding::Meta::getRequiringVersion(ext).toString().c_str());
    }
    cBasicPrint("------------------------------------");

    return true;
}

void CGLBindingRenderer::bindingPreInit()
{
    //Check for extensions! Quick!
//    cMsg("glbinding","Initializing glbinding");
    glbinding::Binding::initialize(true);

//    cMsg("glbinding","Initialized");

}

void CGLBindingRenderer::bindingPostInit()
{
    fetchGLExtensions();
    CGraphicsQuirks::coffee_quirks_set_global(this->m_extensions);

    if(m_properties.contextProperties.flags&CGLContextProperties::GLDebug&&
            m_properties.contextProperties.flags&CGLContextProperties::GLPrintExtensions){
        printExtensions(true);
    }

    try{
        m_rendererString        = glbinding::ContextInfo::renderer();
        m_vendorString          = glbinding::ContextInfo::vendor();
        m_versionString         = glbinding::ContextInfo::version().toString();
        m_libraryRevision       = glbinding::Meta::glRevision();
    }
    catch(std::logic_error e){
        cMsg("glbinding","Failed to acquire GL details");
    }

//    cMsg("glbinding","Obtained context information");

    cMsg("glbinding","Currently running OpenGL revision: %i",m_libraryRevision);

//    glbinding::setCallbackMask(glbinding::CallbackMask::After);

//    glbinding::setAfterCallback([](const glbinding::FunctionCall& call){
//        printf("GL call: %s\n",call.function->name());
//    });

    if(m_properties.contextProperties.flags&CGLContextProperties::GLDebug){
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback((gl::GLDEBUGPROC)glbindingCallbackDirect,this);
    }
}

void CGLBindingRenderer::bindingTerminate()
{
}

void CGLBindingRenderer::bindingCallback(void *report) const
{
    CGLReport* rep = (CGLReport*)report;
    if(!m_msg_filter(rep))
        return;
    CString out = _glbinding_get_string(rep->type)+":"
            +_glbinding_get_string(rep->severity)+":"
            +_glbinding_get_string(rep->source)+": "+rep->message;
    cWarning("OpenGL: %s",out.c_str());
}

cstring CGLBindingRenderer::extensions()
{
    return m_extensions;
}

} // namespace CDisplay
} // namespace Coffee
