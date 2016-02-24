#pragma once

#include <coffee/core/CRegex>
#include <coffee/core/types/composite_types.h>
#include <coffee/core/types/graphics_types.h>
#include <coffee/core/CXmlParser>

namespace Coffee{
namespace CGL{

struct CGLXML
{
    using CoreVer = _cbasic_version<int32>;
    using Extension = CString;
    using ExtList = Vector<Extension>;

    STATICINLINE bool LoadData(cstring fn)
    {
        Lock lock(doc_mutex);

        CResources::CResource xml(fn);
        if(!CResources::FileExists(xml))
            return false;
        CResources::FilePull(xml,true);

        doc = XML::XMLRead(xml);

        CResources::FileFree(xml);

        corever_regx = new Regex::Pattern(Regex::Compile(".*_([0-9])_([0-9])"));

        if(!doc)
            return false;

        return false;
    }
    STATICINLINE bool UnloadData()
    {
        Lock lock(doc_mutex);
        if(doc)
        {
            delete corever_regx;
            delete doc;
            return true;
        }
        return false;
    }
    STATICINLINE ExtList FindExtensions(cstring part)
    {
        ExtList exts;

        auto query = doc->RootElement()
                ->FirstChildElement("extensions")
                ->FirstChildElement("extension");
        while(query)
        {
            cstring extname = query->Attribute("name");
            if(CStrFind(extname,part))
                exts.push_back(CString(extname));
            query = query->NextSiblingElement("extension");
        }

        return exts;
    }
    STATICINLINE CoreVer ExtractCoreVer(cstring s)
    {
        CoreVer out;
        out.major = 0;
        out.minor = 0;
        out.revision = 0;

        auto match = Regex::Match(*corever_regx,s,true);
        if(match.size()!=3)
            return out;

        out.major = Convert::strtoint(match.at(1).s_match.front().c_str());
        out.minor = Convert::strtoint(match.at(2).s_match.front().c_str());

        return out;
    }
    STATICINLINE CoreVer GetFunctionCoreVersion(cstring fun)
    {
        CoreVer out;
        out.major = 0;
        out.minor = 0;
        out.revision = 0;

        auto query = doc->RootElement()->FirstChildElement("feature");
        while(query)
        {
            auto req = query->FirstChildElement("require");
            while(req)
            {
                auto cmd = req->FirstChildElement("command");
                while(cmd)
                {
                    if(CStrCmp(cmd->Attribute("name"),fun))
                        return ExtractCoreVer(query->Attribute("name"));
                    cmd = cmd->NextSiblingElement("command");
                }
                req = req->NextSiblingElement("require");
            }
            query = query->NextSiblingElement("feature");
        }

        return out;
    }
    STATICINLINE Extension GetFunctionExtension(cstring fun)
    {
        auto query = doc->RootElement()
                ->FirstChildElement("extensions")
                ->FirstChildElement("extension");
        while(query)
        {
            auto req = query->FirstChildElement("require");
            while(req)
            {
                auto cmd = req->FirstChildElement("command");
                while(cmd)
                {
                    if(CStrCmp(cmd->Attribute("name"),fun))
                        return Extension(query->Attribute("name"));
                    cmd = cmd->NextSiblingElement("command");
                }
                req = req->NextSiblingElement("require");
            }
            query = query->NextSiblingElement("extension");
        }

        return Extension();
    }
    STATICINLINE ExtList GetCoreExtensions(CoreVer ver)
    {
        ExtList exts;
        if(ver.major<=3&&ver.minor<3)
            return exts;


        return exts;
    }
private:
    static Regex::Pattern* corever_regx;
    static XML::Document* doc;
    static Mutex doc_mutex;
};

}
}