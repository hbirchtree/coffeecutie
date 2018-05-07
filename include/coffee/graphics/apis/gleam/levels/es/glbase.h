#ifndef COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_BASE_H
#define COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_BASE_H

#include <coffee/graphics/common/gltypes.h>

#include <coffee/core/coffee_strings.h>
#include <coffee/core/plat/memory/cmemory.h>

#include "../gl_shared.h"

namespace Coffee{
namespace CGL{

struct CGL_Implementation : CGL_Shared_Functions
{
    /* Base OpenGL, all implemented in GL3.3 */

    using Debug = CGL_Shared_Functions::Debug;
};

}
}

#endif

