#include <coffee/graphics_apis/opengl/glbinding/cglbindingrenderer.h>

#include <coffee/core/CDataStorage>
#include <coffee/graphics_apis/COpenGL>

#include <glbinding/ContextInfo.h>
#include <glbinding/Version.h>
#include <glbinding/callbacks.h>
#include <glbinding/Binding.h>
#include <glbinding/Meta.h>

#include <coffee/graphics_apis/opengl/glfunctions.h>

using namespace Coffee::CDisplay;

namespace Coffee {
namespace CDisplay {

using namespace CGraphicsWrappers;

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

void CGLBindingRenderer::bindingPreInit(const GLProperties&)
{
    glbinding::Binding::initialize(true);
}

void CGLBindingRenderer::bindingInit(const GLProperties&)
{
}

std::set<CString> functions;

void CGLBindingRenderer::bindingPostInit(const GLProperties & p)
{
    //Check for extensions! Quick!
    CString m_extensions = coffee_graphics_get_extensions(nullptr);
    CGraphicsQuirks::coffee_quirks_set_global(m_extensions.data());

    try{
        m_rendererString        = coffee_graphics_context_get_renderer();
        m_vendorString          = coffee_graphics_context_get_vendor();
        m_versionString         = coffee_graphics_context_get_version_string();
        m_libraryRevision       = glbinding::Meta::glRevision();
    }
    catch(std::logic_error e){
        cMsg("glbinding","Failed to acquire GL details");
    }

    cMsg("glbinding","Renderer={0}, vendor={1}, version={2}",
         m_rendererString.c_str(),
         m_vendorString.c_str(),
         m_versionString.c_str());
    cMsg("glbinding","Currently running OpenGL revision: {0}",m_libraryRevision);

    if(p.flags&GLProperties::GLFeatureLevelProfile)
    {
        glbinding::setCallbackMask(glbinding::CallbackMask::After);

        glbinding::setAfterCallback([](const glbinding::FunctionCall& call){
            functions.insert(CString(call.function->name()));
        });
    }

    if(p.flags&GLProperties::GLDebug)
    {
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
    cWarning("OpenGL: {0}",rep.message.c_str());
}

std::map<CString,CString> coffee_glbinding_get_graphics_feature_level()
{
    CResources::Resource glxml("gl.xml");

    if(!CResources::FileExists(glxml))
        return std::map<CString,CString>();

    CResources::FilePull(glxml);

    const XML::Document* doc =
            XML::XMLRead(glxml);

    const XML::Element* feature =
            doc->RootElement()->FirstChildElement("feature");

    const XML::Element* extension =
            doc->RootElement()
            ->FirstChildElement("extensions")
            ->FirstChildElement("extension");

    std::map<CString,cstring> feature_levels;

    //First, we query the core version
    auto query = feature;
    while(query)
    {
        auto subf = query->FirstChildElement("require");
        while(subf)
        {
            auto command = subf->FirstChildElement("command");
            while(command)
            {
                for(const CString& f : functions)
                    if(!feature_levels[f])
                        if(StrCmp(command->Attribute("name"),f.c_str()))
                        {
                            feature_levels[f] = query->Attribute("name");
                        }
                command = command->NextSiblingElement("command");
            }
            subf = subf->NextSiblingElement("require");
        }
        query = query->NextSiblingElement("feature");
    }
    bool finished = true;

    for(const CString& f : functions)
        if(!feature_levels[f])
        {
            finished = false;
            break;
        }

    //If we cannot find a core version, find the extension
    if(!finished)
    {
        auto ext = extension;
        while(ext)
        {
            auto cmd = ext->FirstChildElement("require");
            if(cmd)
            {
                auto itm = cmd->FirstChildElement("command");
                while(itm)
                {
                    for(const CString& f : functions)
                        if(!feature_levels[f])
                            if(StrCmp(itm->Attribute("name"),f.c_str()))
                            {
                                feature_levels[f] = ext->Attribute("name");
                            }
                    itm = itm->NextSiblingElement("command");
                }
            }
            ext = ext->NextSiblingElement("extension");
        }
    }

    std::map<CString,CString> features;

    for(const std::pair<CString,cstring>& ft : feature_levels)
    {
        features[ft.first] = ft.second;
    }

    CResources::FileFree(glxml);

    delete doc;

    return features;
}

} // namespace CDisplay
} // namespace Coffee
