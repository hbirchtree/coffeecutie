#pragma once

#include "gl_shared_include.h"
#include "gl_shared_types.h"

#include <coffee/core/base/types/cdisplay.h>
#include <coffee/core/plat/memory/string_ops.h>
#include <coffee/core/base/textprocessing/cregex.h>

namespace Coffee{
namespace CGL{

struct CGL_Shared_Debug
{
    static bool b_isDebugging;
    static CString s_ExtensionList;

    /* Extensions */

    STATICINLINE void GetExtensions()
    {
        int32 numExtensions = GetInteger(GL_NUM_EXTENSIONS);

        if (numExtensions <= 0)
            return;

        s_ExtensionList = std::string();
        s_ExtensionList.reserve(numExtensions*20);
        for(int32 i=0;i<numExtensions;i++)
        {
            cstring str = GetStringi(GL_EXTENSIONS,i);
            s_ExtensionList.append(str);
            if(i<numExtensions-1)
                s_ExtensionList.push_back(' ');
        }
    }

    STATICINLINE bool CheckExtensionSupported(cstring id)
    {
        return Mem::Search::StrFind(s_ExtensionList.c_str(),id);
    }

    /* Context information */

    STATICINLINE CDisplay::CGLVersion ContextVersion()
    {
        CDisplay::CGLVersion ver = {};

        ver.major = 0;
        ver.minor = 0;
//            ver.major = GetInteger(GL_MAJOR_VERSION);
//            ver.minor = GetInteger(GL_MINOR_VERSION);
        ver.revision = 0;

        /* In some cases, we run on drivers that are old or crappy */
        if (ver.major == 0 && ver.minor == 0)
        {
            CString str = GetString(GL_VERSION);
            if (str.size()<=0)
                return ver;
            Regex::Pattern p = Regex::Compile("([0-9]+)\\.([0-9]+)\\.([0-9])?([\\s\\S]*)");
            auto match = Regex::Match(p,str,true);
            if (match.size() < 3)
                return ver;
            ver.major = Convert::strtoint(match.at(1).s_match.front().c_str());
            ver.minor = Convert::strtoint(match.at(2).s_match.front().c_str());
            if (match.size() == 4)
            {
                bool ok = false;
                ver.revision = Convert::strtoint(match.at(3).s_match.front().c_str(),10,&ok);
                if (!ok)
                {
                    ver.driver = match.at(3).s_match.front();
                }
            }
            else if (match.size() == 5)
            {
                ver.revision = Convert::strtoint(match.at(3).s_match.front().c_str());
                ver.driver = StrUtil::trim(match.at(4).s_match.front());
            }
        }

        return ver;
    }

    /* GLSL information */

    STATICINLINE CDisplay::CGLVersion ShaderLanguageVersion()
    {
        CDisplay::CGLVersion ver = {};

        CString str = GetString(GL_SHADING_LANGUAGE_VERSION);

        ver.driver = str;

        if(str.size()<1)
            return ver;

        Regex::Pattern p = Regex::Compile("([0-9]+)\\.([0-9]+).([\\s\\S]*)");
        auto match = Regex::Match(p,str,true);

        if(match.size() < 3)
            return ver;

        ver.major = Convert::strtoint(match.at(1).s_match.front().c_str());
        ver.minor = Convert::strtoint(match.at(2).s_match.front().c_str());

        if(match.size() < 4)
            return ver;

        ver.driver = match.at(3).s_match.front().c_str();

        return ver;
    }

    /* Rendering device info */

    STATICINLINE HWDeviceInfo Renderer()
    {
        cstring vendor = GetString(GL_VENDOR);
        cstring device = GetString(GL_RENDERER);
        cstring ver = GetString(GL_VERSION);

        return HWDeviceInfo(vendor,device,ver);
    }

    /* Texture format support */

    STATICINLINE int32* InternalFormatSupport(CGenum tt, CGenum t, CGenum prop,int32 n)
    {
        int32* i = new int32[n];
        glGetInternalformativ(tt,t,prop,n*sizeof(i[0]),i);
        return i;
    }


    /* GetString */

    STATICINLINE cstring GetString(CGenum e){return (cstring)glGetString(e);}
    STATICINLINE cstring GetStringi(CGenum e,uint32 i){return (cstring)glGetStringi(e,i);}

    /* Get*v */

    STATICINLINE int32 GetInteger(CGenum e)
    {
        int32 i = 0;
        glGetIntegerv(e,&i);
        return i;
    }

    STATICINLINE int64 GetIntegerLL(CGenum e)
    {
        int64 i = 0;
        glGetInteger64v(e,&i);
        return i;
    }

    STATICINLINE scalar GetScalar(CGenum e)
    {
        scalar i = 0.f;
        glGetFloatv(e,&i);
        return i;
    }

    STATICINLINE bool GetBooleanv(CGenum e)
    {
        GLboolean i = GL_FALSE;
        glGetBooleanv(e,&i);
        return i==GL_TRUE;
    }

    /* Get*i_v */

    STATICINLINE int32 GetIntegerI(CGenum e,uint32 i)
    {
        int32 v = 0;
        glGetIntegeri_v(e,i,&v);
        return v;
    }

    STATICINLINE int64 GetIntegerLLI(CGenum e,uint32 i)
    {
        int64 v = 0;
        glGetInteger64i_v(e,i,&v);
        return v;
    }

    /* Object validity */

    STATICINLINE
    bool IsBuffer(CGhnd h)
    {
        return glIsBuffer(h)==GL_TRUE;
    }
    STATICINLINE
    bool IsVAO(CGhnd h){return glIsVertexArray(h)==GL_TRUE;}

    STATICINLINE
    bool IsFramebuffer(CGhnd h){return glIsFramebuffer(h)==GL_TRUE;}
    STATICINLINE
    bool IsRenderbuffer(CGhnd h){return glIsRenderbuffer(h)==GL_TRUE;}

    STATICINLINE
    bool IsShader(CGhnd h){return glIsShader(h)==GL_TRUE;}
    STATICINLINE
    bool IsProgram(CGhnd h){return glIsProgram(h)==GL_TRUE;}

    STATICINLINE
    bool IsTexture(CGhnd h){return glIsTexture(h)==GL_TRUE;}
    STATICINLINE
    bool IsSampler(CGhnd h){return glIsSampler(h)==GL_TRUE;}

    STATICINLINE
    bool IsSync(CGsync h){return glIsSync((GLsync)h)==GL_TRUE;}
    STATICINLINE
    bool IsQuery(CGhnd h){return glIsQuery(h)==GL_TRUE;}

    STATICINLINE
    bool IsXF(CGhnd h){return glIsTransformFeedback(h)==GL_TRUE;}
};

}
}
