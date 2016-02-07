#include <coffee/graphics_apis/opengl/glbinding/glbinding.h>

#include <glbinding/Meta.h>

namespace Coffee{
namespace CGraphicsWrappers{

CString _glbinding_get_string(GLenum e)
{
    return glbinding::Meta::getString(e);
}

CString _glbinding_get_string(GLextension e)
{
    return glbinding::Meta::getString(e);
}

}
}
