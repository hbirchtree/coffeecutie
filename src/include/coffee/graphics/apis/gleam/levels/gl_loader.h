#pragma once

#include "shared/gl_shared_include.h"
#include "shared/gl_shared_debug.h"
#include <coffee/graphics/common/gltypes.h>
#include <coffee/core/coffee_mem_macros.h>

namespace Coffee {
namespace CGL {

struct Loader
{
    STATICINLINE bool LoadBinding(
        CGL_Context* ctxt, GLADloadproc fun, void** ptr_check)
    {
        C_USED(fun);

        if(!ctxt->acquireContext())
            return false;

#if !defined(COFFEE_GLEAM_DESKTOP) && !defined(COFFEE_LINKED_GLES)
        if(!gladLoadGLES2Loader(fun))
            return false;
#else
        if(!gladLoadGL())
            return false;
#endif

        if(!glGetString(GL_VENDOR))
            return false;

        return static_cast<bool>(*ptr_check);
    }
};

using Debug = CGL_Shared_Debug;

} // namespace CGL
} // namespace Coffee
