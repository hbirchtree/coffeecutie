#include "cglbindingrenderer.h"

#include <coffee/graphics_apis/COpenGL>

#include <glbinding/ContextInfo.h>
#include <glbinding/Version.h>
#include <glbinding/callbacks.h>
#include <glbinding/Binding.h>
#include <glbinding/Meta.h>

#include "glbinding.h"

using namespace gl;
using namespace Coffee::CDisplay;

namespace Coffee {
namespace CDisplay {

void glbinding_default_callback(CGLDebugMessage const& report, void* userPtr)
{
    CGLBindingRenderer* ptr = (CGLBindingRenderer*)userPtr;
    ptr->bindingCallback(&report);
}

CGLBindingRenderer::CGLBindingRenderer(Coffee::CObject *parent) :
    CSDL2Renderer(parent),
    m_msg_filter(nullptr)
{
}

CGLBindingRenderer::~CGLBindingRenderer()
{
}

void CGLBindingRenderer::bindingPreInit()
{
//    cMsg("glbinding","Initializing glbinding");
    glbinding::Binding::initialize(true);

//    cMsg("glbinding","Initialized");

}

void CGLBindingRenderer::bindingPostInit()
{
    //Check for extensions! Quick!
    CString m_extensions = coffee_graphics_get_extensions(nullptr);
    CGraphicsQuirks::coffee_quirks_set_global(m_extensions.data());

    if(m_properties.contextProperties.flags&CGLContextProperties::GLDebug&&
            m_properties.contextProperties.flags&CGLContextProperties::GLPrintExtensions){
//        printExtensions(true);
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
        coffee_graphics_debug_context(true,glbinding_default_callback,this);
    }
}

void CGLBindingRenderer::bindingTerminate()
{
}

void CGLBindingRenderer::bindingCallback(const void *report) const
{
    CGLDebugMessage const& rep = *((CGLDebugMessage*)report);
    if(!m_msg_filter(rep))
        return;
    CString out = "";
    cWarning("OpenGL: %s",out.c_str());
}

} // namespace CDisplay
} // namespace Coffee
