#ifndef COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_BASE_H
#define COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_BASE_H

#include <coffee/core/CDebug>

#include "../gl_shared.h"

#include "../glxml.h"

namespace Coffee{
namespace CGL{

struct CGL_Implementation : CGL_Shared_Functions
{
    /* Base OpenGL, all implemented in GL3.3 */


    /* SAMPLE_ALPHA_TO_COVERAGE,SAMPLE_ALPHA_TO_ONE */

    struct Debug : CGL_Shared_Functions::Debug
    {

        /* Get*v */

        STATICINLINE bigscalar GetScalarLL(CGenum e)
        {
            bigscalar i = 0.0;
            glGetDoublev(e,&i);
            return i;
        }

        /* Get*i_v */

        STATICINLINE scalar GetScalarI(CGenum e,uint32 i)
        {
            scalar v = 0.f;
            glGetFloati_v(e,i,&v);
            return v;
        }
        STATICINLINE bigscalar GetScalarLLI(CGenum e,uint32 i)
        {
            bigscalar v = 0.0;
            glGetDoublei_v(e,i,&v);
            return v;
        }

        STATICINLINE bool GetBooleanvI(CGenum e,uint32 i)
        {
            GLboolean v = GL_FALSE;
            glGetBooleani_v(e,i,&v);
            return v==GL_TRUE;
        }
    };

};

}
}

#endif

#include "../gl_to_enum.inl"
