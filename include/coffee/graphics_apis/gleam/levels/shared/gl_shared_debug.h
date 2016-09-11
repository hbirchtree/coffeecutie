#pragma once

#include "gl_shared_include.h"
#include "gl_shared_types.h"
#include "gl_shared_enum_convert.h"

#include <coffee/core/base/types/cdisplay.h>
#include <coffee/core/plat/memory/string_ops.h>
#include <coffee/core/base/textprocessing/cregex.h>

namespace Coffee{
namespace CGL{

struct CGL_Shared_Debug
{
    static bool b_isDebugging;
    static CString s_ExtensionList;

    /* Stores information on pixel formats */
    static int32 Num_Internal_Formats;
    static int32* Internal_Formats;

    /* Verifying loader results */
    STATICINLINE bool VerifyInit()
    {
        return (bool)glEnable;
    }

    /* GL_KHR_debug */
    STATICINLINE void InitDebugFlag()
    {
        b_isDebugging = false;
    }

    STATICINLINE void SetDebugMode(bool enabled)
    {
        if(enabled == b_isDebugging)
            return;
        if(enabled)
        {
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            b_isDebugging = true;
        }
        else
        {
            glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            b_isDebugging = true;
        }
    }

    STATICINLINE void SetDebugLevel(Severity s,bool enabled)
    {
        glDebugMessageControl(
                    GL_DONT_CARE,GL_DONT_CARE,
                    to_enum(s),0,nullptr,
                    (enabled)?GL_TRUE:GL_FALSE);
    }

    STATICINLINE void SetObjectLabel(Object t,CGhnd h,cstring s)
    {
        glObjectLabel(to_enum(t),h,-1,s);
    }

    STATICINLINE void SetDebugGroup(cstring n, uint32 id)
    {
        glPushDebugGroup(GL_DEBUG_TYPE_PUSH_GROUP,id,-1,n);
    }
    STATICINLINE void UnsetDebugGroup()
    {
        glPopDebugGroup();
    }

    STATICINLINE void DebugMessage(Severity s,DebugType t,cstring n)
    {
        glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION,
                             to_enum(t),0,
                             to_enum(s),
                             -1,n);
    }

    STATICINLINE void DebugSetCallback(CGcallback c, void* param)
    {
        glDebugMessageCallback(c,param);
    }

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

    STATICINLINE Display::CGLVersion ContextVersion()
    {
        Display::CGLVersion ver = {};

        ver.major = 0;
        ver.minor = 0;
        ver.revision = 0;

        /* In most cases, this will work wonders */
        do
        {
            ver.major = GetInteger(GL_MAJOR_VERSION);
            ver.minor = GetInteger(GL_MINOR_VERSION);
        }while(false);

        /* In some cases, we run on drivers that are old or crappy.
         * We still want to know what the hardware supports, though. */
        do
        {
            CString str = GetString(GL_VERSION);

            cVerbose(7,"Input GL_VERSION string: {0}",str);

            if (str.size()<=0)
                break;
            Regex::Pattern p = Regex::Compile("([0-9]+)\\.([0-9]+)\\.([0-9])?([\\s\\S]*)");
            auto match = Regex::Match(p,str,true);
            if (match.size() < 3)
                break;
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
        }while(false);

        return ver;
    }

    /* GLSL information */

    STATICINLINE Display::CGLVersion ShaderLanguageVersion()
    {
        Display::CGLVersion ver = {};

        cstring str_c = GetString(GL_SHADING_LANGUAGE_VERSION);

        if (!str_c)
            return ver;

        cVerbose(7,"Input GL_SHADING_LANGUAGE_VERSION string: {0}",str_c);

        CString str = str_c;

        ver.driver = str;

        if(str.size()<1)
            return ver;

        if(ver.major == 0 && ver.minor ==0)
            do
            {
                Regex::Pattern p = Regex::Compile("([0-9]+)\\.([0-9]+).([\\s\\S]*)");
                auto match = Regex::Match(p,str,true);

                if(match.size() < 3)
                    break;

                ver.major = Convert::strtoint(match.at(1).s_match.front().c_str());
                ver.minor = Convert::strtoint(match.at(2).s_match.front().c_str());

                if(match.size() < 4)
                    break;

                ver.driver = match.at(3).s_match.front().c_str();

            }while(false);

        if(ver.major == 0 && ver.minor == 0)
            do
            {
                Regex::Pattern pa = Regex::Compile("([\\s\\S]+) ([0-9]+).([0-9]+)");
                auto match = Regex::Match(pa,str,true);

                if(match.size() == 3)
                {
                    ver.major = Convert::strtoint(match.at(1).s_match.front().c_str());
                    ver.minor = Convert::strtoint(match.at(2).s_match.front().c_str());
                    break;
                }else if(match.size() ==4)
                {
                    ver.major = Convert::strtoint(match.at(2).s_match.front().c_str());
                    ver.minor = Convert::strtoint(match.at(3).s_match.front().c_str());
                }else
                    break;

                ver.driver = match.at(1).s_match.front().c_str();

            }while(false);

        return ver;
    }

    /* Rendering device info */

    STATICINLINE HWDeviceInfo Renderer()
    {
        cstring vendor = GetString(GL_VENDOR);
        cstring device = GetString(GL_RENDERER);

        return HWDeviceInfo(vendor,device,ContextVersion().driver);
    }

    /* Texture format support */
    STATICINLINE void InitInternalFormats()
    {
    }
    STATICINLINE void FreeInternalFormats()
    {
    }

    STATICINLINE void InitCompressedFormats()
    {
        Num_Internal_Formats = GetInteger(GL_NUM_COMPRESSED_TEXTURE_FORMATS);
        Internal_Formats = new int32[Num_Internal_Formats];
        GetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS,Internal_Formats);
    }

    STATICINLINE void FreeCompressedFormats()
    {
        Num_Internal_Formats = 0;
        delete[] Internal_Formats;
    }

    STATICINLINE bool CompressedFormatSupport(Texture, PixelFormat t)
    {
        /* TODO: GL_COMPRESSED_TEXTURE_FORMATS */
        int32 supp = GL_FALSE;
        CGenum target = to_enum(t);
        for(int32 i=0;i<Num_Internal_Formats;i++)
            if(target == Internal_Formats[i])
            {
                supp = GL_TRUE;
                break;
            }
        return supp == GL_TRUE;
    }
    STATICINLINE ColBits CompressedFormatDepths(Texture, PixelFormat)
    {
//        int32 supp;
//        glGetInternalformativ(to_enum(tt),to_enum(t),GL_INTERNALFORMAT_SUPPORTED,sizeof(supp),&supp);
        return {};
    }

    STATICINLINE bool InternalFormatSupport(Texture, PixelFormat)
    {
        return true;
    }
    STATICINLINE CSize InternalFormatMaxResolution2D(Texture tt, PixelFormat t)
    {
        CSize sz;
        sz.w = sz.h = GetInteger(GL_MAX_TEXTURE_SIZE);
        return sz;
    }


    /* GetString */

    STATICINLINE cstring GetString(CGenum e){return (cstring)glGetString(e);}
    STATICINLINE cstring GetStringi(CGenum e,uint32 i){return (cstring)glGetStringi(e,i);}

    /* Get*v */

    STATICINLINE CSize GetViewport()
    {
        CRect r;
        GetIntegerv(GL_VIEWPORT,r.data);
        return r.size();
    }

    STATICINLINE void GetIntegerv(CGenum e, int32* v)
    {
        glGetIntegerv(e,v);
    }
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

    STATICINLINE
    bool IsPipeline(CGhnd h){return glIsProgramPipeline(h)==GL_TRUE;}

    /* IsEnabled */
    STATICINLINE
    bool IsEnabledi(Feature f,int32 i){return glIsEnabledi(to_enum(f),i)==GL_TRUE;}
};

}
}
