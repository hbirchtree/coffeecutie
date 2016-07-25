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

