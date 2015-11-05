#ifndef GLBINDING
#define GLBINDING

#include "coffee/core/coffee_types.h"

#include <glbinding/ContextInfo.h>
#include <glbinding/Version.h>
#include <glbinding/callbacks.h>
#include <glbinding/Binding.h>
#include <glbinding/Meta.h>

#include <glbinding/gl/gl.h>

using namespace gl;

namespace Coffee{
namespace CGraphicsWrappers{

typedef const GLchar* cglstring;

template<typename GT>
CString _glbinding_get_string(GT e)
{
    return glbinding::Meta::getString(e);
}

}
}

#endif // GLBINDING

