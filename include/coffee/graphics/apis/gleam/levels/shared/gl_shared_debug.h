#pragma once

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
        return C_CAST<bool>(glEnable);
    }

    /* GL_KHR_debug */
    STATICINLINE void InitDebugFlag()
    {
        b_isDebugging = false;
    }

    static void SetDebugMode(bool enabled);

    STATICINLINE void SetDebugLevel(Severity s,bool enabled)
    {
#if !defined(COFFEE_ONLY_GLES20)
        glDebugMessageControl(
                    GL_DONT_CARE,GL_DONT_CARE,
                    to_enum(s),0,nullptr,
                    (enabled)?GL_TRUE:GL_FALSE);
#endif
    }

    STATICINLINE void SetObjectLabel(Object t,CGhnd h,cstring s)
    {
#if !defined(COFFEE_ONLY_GLES20)
        glObjectLabel(to_enum(t),h,-1,s);
#endif
    }

    STATICINLINE void SetDebugGroup(cstring n, uint32 id)
    {
#if !defined(COFFEE_ONLY_GLES20)
        glPushDebugGroup(GL_DEBUG_TYPE_PUSH_GROUP,id,-1,n);
#endif
    }
    STATICINLINE void UnsetDebugGroup()
    {
#if !defined(COFFEE_ONLY_GLES20)
        glPopDebugGroup();
#endif
    }

    STATICINLINE void DebugMessage(Severity s,DebugType t,cstring n)
    {
#if !defined(COFFEE_ONLY_GLES20)
        glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION,
                             to_enum(t),0,
                             to_enum(s),
                             -1,n);
#endif
    }

    STATICINLINE void DebugSetCallback(CGcallback c, void* param)
    {
#if !defined(COFFEE_ONLY_GLES20)
        glDebugMessageCallback(c,param);
#endif
    }

    /* Extensions */

    static void GetExtensions();

    STATICINLINE bool CheckExtensionSupported(cstring id)
    {
        return Mem::Search::StrFind(s_ExtensionList.c_str(),id);
    }

    /* Context information */

    static Display::CGLVersion ContextVersion();

    /* GLSL information */

    static Display::CGLVersion ShaderLanguageVersion();

    /* Rendering device info */

    STATICINLINE HWDeviceInfo Renderer()
    {
        cstring vendor = GetString(GL_VENDOR);
        cstring device = GetString(GL_RENDERER);

        if(!vendor || !device)
            return HWDeviceInfo("Unknown", "Unknown", "");

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

    static bool CompressedFormatSupport(Texture, PixelFormat t);

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
#if !defined(COFFEE_ONLY_GLES20)
    STATICINLINE cstring GetStringi(CGenum e,uint32 i){return (cstring)glGetStringi(e,i);}
#endif

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
#if !defined(COFFEE_ONLY_GLES20)
    STATICINLINE int64 GetIntegerLL(CGenum e)
    {
        int64 i = 0;
        glGetInteger64v(e,&i);
        return i;
    }
#endif

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
#if !defined(COFFEE_ONLY_GLES20)
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
#endif

    /* Object validity */

    STATICINLINE
    bool IsBuffer(CGhnd h)
    {
        return glIsBuffer(h)==GL_TRUE;
    }
#if !defined(COFFEE_ONLY_GLES20)
    STATICINLINE
    bool IsVAO(CGhnd h){return glIsVertexArray(h)==GL_TRUE;}
#endif

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

#if !defined(COFFEE_ONLY_GLES20)
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
#endif
};

}
}
