#ifndef GLBINDING
#define GLBINDING

#include "coffee/core/coffee_types.h"

#include <glbinding/gl/gl.h>

using namespace gl;

namespace Coffee{
namespace CGraphicsWrappers{

C_FLAGS(UseProgramStageMask,uint32)

typedef const GLchar* cglstring;

extern CString _glbinding_get_string(GLenum e);
extern CString _glbinding_get_string(GLextension e);

struct CGHndl
{
    GLuint handle;
};

}
}

#endif // GLBINDING

