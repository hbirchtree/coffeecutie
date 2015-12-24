#ifndef COFFEE_GRAPHICS_APIS_OPENGL_FUNCTIONS_H
#define COFFEE_GRAPHICS_APIS_OPENGL_FUNCTIONS_H

#include <coffee/core/types/basetypes.h>

/* What should an OpenGL function set contain:
 *
 *  - gl_get,gl_getf,gl_geti to translate GLenum to Coffee types
 *  - gl_get_dbg_* functions for debug translation
 *  - GL functions. Obviously.
 *
 * All of this should be contained within the CGraphicsWrappers namespace.
 */

#include "glbinding/glbinding.h"

namespace CGL
{

}

#endif
