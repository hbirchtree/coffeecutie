#include <glbinding/cglbindingrenderer.h>

#include <coffee/core/CDataStorage>
#include <coffee/graphics_apis/COpenGL>

#include <glbinding/ContextInfo.h>
#include <glbinding/Version.h>
#include <glbinding/callbacks.h>
#include <glbinding/Binding.h>
#include <glbinding/Meta.h>

#include <glfunctions.h>

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

std::set<CString> functions;

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

    cMsg("glbinding","Currently running OpenGL revision: {0}",m_libraryRevision);

    if(m_properties.contextProperties.flags&CGLContextProperties::GLFeatureLevelProfile)
    {
        glbinding::setCallbackMask(glbinding::CallbackMask::After);

        glbinding::setAfterCallback([](const glbinding::FunctionCall& call){
            functions.insert(CString(call.function->name()));
        });
    }

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
    cWarning("OpenGL: {0}",rep.message.c_str());
}

std::map<CString,CString> coffee_glbinding_get_graphics_feature_level()
{
    CResources::CResource glxml("gl.xml");

    if(!CResources::coffee_file_exists(glxml))
        return std::map<CString,CString>();

    CResources::coffee_file_pull(glxml);

    const CDataStorage::CTextStorage::XMLDocument* doc =
            CDataStorage::CTextStorage::coffee_xml_read(glxml);

    const CDataStorage::CTextStorage::XMLElement* feature =
            doc->RootElement()->FirstChildElement("feature");

    const CDataStorage::CTextStorage::XMLElement* extension =
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
                        if(c_strcmp(command->Attribute("name"),f.c_str()))
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
                            if(c_strcmp(itm->Attribute("name"),f.c_str()))
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

    CResources::coffee_file_free(glxml);

    return features;
}

} // namespace CDisplay
} // namespace Coffee
