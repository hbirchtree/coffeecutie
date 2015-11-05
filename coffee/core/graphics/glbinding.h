#ifndef GLBINDING
#define GLBINDING

#include "coffee/core/coffee_types.h"

#include <glbinding/gl/gl.h>

using namespace gl;

namespace Coffee{
namespace CGraphicsWrappers{

typedef const GLchar* cglstring;

extern CString _glbinding_get_string(GLenum e);
extern CString _glbinding_get_string(GLextension e);

}
}

#endif // GLBINDING

