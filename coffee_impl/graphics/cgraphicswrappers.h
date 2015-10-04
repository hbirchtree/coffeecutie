#ifndef CGRAPHICSWRAPPERS_H
#define CGRAPHICSWRAPPERS_H

#include "cvertexarrayobject.h"
#include "cuniformtypes.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct CGLReport
{
    GLenum source;
    GLenum type;
    GLuint id;
    GLenum severity;
    const char* message;
};

struct CGLDrawCall
{
    GLuint count            = 0;
    GLuint instanceCount    = 0;
    GLuint firstIndex       = 0;
    GLuint baseVertex       = 0;
    GLuint baseInstance     = 0;
};

}
}

#endif // CGRAPHICSWRAPPERS_H
