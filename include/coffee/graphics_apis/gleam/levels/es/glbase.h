#ifndef COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_BASE_H
#define COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_BASE_H

#include "../../../gltypes.h"

#include <coffee/core/coffee_strings.h>
#include <coffee/core/CDebug>
#include <coffee/core/plat/memory/cmemory.h>

#include "../gl_shared.h"

namespace Coffee{
namespace CGL{

struct CGL_Implementation : CGL_Shared_Functions
{
    /* Base OpenGL, all implemented in GL3.3 */
    static void Enable(Feature e)
    {glEnable(to_enum(e));}
    static void Disable(Feature e)
    {glDisable(to_enum(e));}

    STATICINLINE void Enable(Feature e,uint32 i,uint32 o = 0){glEnablei(to_enum(e,o),i);}
    STATICINLINE void Disable(Feature e,uint32 i,uint32 o = 0){glDisablei(to_enum(e,o),i);}

    STATICINLINE void ColorMaski(uint32 i, CColorMask op){glColorMaski(i,op.r,op.g,op.b,op.a);}

    STATICINLINE void ColorLogicOp(LogicOp){}
    STATICINLINE void DepthSet(ZField64 const*){}

    STATICINLINE void PointParameteriv(CGenum,const int32*){}
    STATICINLINE void PointParameterfv(CGenum,const scalar*){}
    STATICINLINE void PointSize(scalar){}

    STATICINLINE void PolyMode(Face,DrawMode){}

    static bool DebuggingSupported()
    {return Debug::CheckExtensionSupported("GL_KHR_debug");}

    struct Debug : CGL_Shared_Functions::Debug
    {
	static bool VerifyInit()
	{
	    return (bool)glEnable;
        }

        //Variables

        /* Strictly debugging */

        static void InitDebugFlag(){b_isDebugging = false;}

        static void SetDebugMode(bool enabled)
        {
        }

        static void SetDebugLevel(Severity s,bool e)
        {
        }

        static void SetObjectLabel(Object t,CGhnd h,cstring s)
        {
        }

        static void SetDebugGroup(cstring n, uint32 id)
        {
        }
        static void UnsetDebugGroup(){}

        static void DebugMessage(Severity s,DebugType t,cstring n)
        {
        }

        static void DebugSetCallback(CGcallback, void*)
        {
        }

        /* Features */

        static void IsEnabled(Feature f)
        {
            glIsEnabled(to_enum(f));
        }
    };

    static void LoadBinding(){}
};

}
}

#endif

