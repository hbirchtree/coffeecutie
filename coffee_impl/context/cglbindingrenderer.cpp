#include "cglbindingrenderer.h"
#include "coffee_impl/graphics/cgraphicswrappers.h"

using namespace gl;
using namespace Coffee::CDisplay;


namespace Coffee {
namespace CDisplay {

static void glbindingCallbackDirect(GLenum source, GLenum type,GLuint id, GLenum severity,GLsizei length, const GLchar* msg,const void* userPtr);

static void glbindingCallbackDirect(GLenum source, GLenum type,
                                             GLuint id, GLenum severity,
                                             GLsizei length, const GLchar* msg,
                                             const void* userPtr)
{
    const CGLBindingRenderer* renderer = static_cast<const CGLBindingRenderer*>(userPtr);
    CGLReport *report = reinterpret_cast<CGLReport*>(malloc(sizeof(CGLReport)));
    report->source = source;
    report->type = type;
    report->id = id;
    report->message = msg;
    report->severity = severity;
    renderer->bindingCallback(report);
}

CGLBindingRenderer::CGLBindingRenderer(Coffee::CObject *parent) :
    CSDL2Renderer(parent)
{
}

CGLBindingRenderer::~CGLBindingRenderer()
{
}

void CGLBindingRenderer::fetchGLExtensions()
{
    GLint exts = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS,&exts);

    CString tmp;

    for(GLint i=0;i<exts;i++){
        tmp.append((byte*)glGetStringi(GL_EXTENSIONS,i));
        if(i<exts-1)
            tmp.push_back('\n');
    }

    cstring_w extensions = (cstring_w)malloc(tmp.size()+1);
    strcpy(extensions,tmp.c_str());
    m_extensions = extensions;
}

bool CGLBindingRenderer::requestGLExtension(cstring ext)
{
    if(!m_extensions){
        if(std::this_thread::get_id()==contextThread())
            fetchGLExtensions();
        else{
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

void CGLBindingRenderer::bindingPreInit()
{
    //Check for extensions! Quick!
    cMsg("glbinding","Initializing glbinding");
    glbinding::Binding::initialize(true);

    cMsg("glbinding","Initialized");

}

void CGLBindingRenderer::bindingPostInit()
{
    if(m_properties.contextProperties.flags&CGLContextProperties::GLDebug){
        cDebug("Now printing extensions available to glbinding (Not necessarily supported by your hardware!)");
        cBasicPrint("-----------| Extensions |-----------");
        for(GLextension ext : glbinding::Meta::extensions()){
            cstring extname = glbinding::Meta::getString(ext).c_str();
            cBasicPrint("Extension: %s, required version: %s, supported: %i",
                        extname,
                        glbinding::Meta::getRequiringVersion(ext).toString().c_str(),
                        requestGLExtension(extname));
        }
        cBasicPrint("------------------------------------");
    }

    m_rendererString        = glbinding::ContextInfo::renderer();
    m_vendorString          = glbinding::ContextInfo::vendor();
    m_versionString         = glbinding::ContextInfo::version().toString();
    m_libraryRevision       = glbinding::Meta::glRevision();

    cMsg("glbinding","Obtained context information");

    cMsg("glbinding","Currently running OpenGL revision: %i",m_libraryRevision);

//    glbinding::setCallbackMask(glbinding::CallbackMask::After);

//    glbinding::setAfterCallback([](const glbinding::FunctionCall& call){
//        printf("GL call: %s\n",call.function->name());
//    });

    if(m_properties.contextProperties.flags&CGLContextProperties::GLDebug){
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glbindingCallbackDirect,this);
    }
}

void CGLBindingRenderer::bindingTerminate()
{
}

} // namespace CDisplay
} // namespace Coffee
