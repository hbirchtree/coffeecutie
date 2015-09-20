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

struct CGLState
        //A partial state dump of OpenGL bindings and etc.
{
    GLuint      vertex_array    = 0;
    GLuint      array_buffer    = 0;
    GLuint      uniform_buffer  = 0;
    GLuint      element_buffer  = 0;
    GLuint      program_obj     = 0;
    GLuint      pipeline_obj    = 0;

    GLuint      indirect_buffer = 0;

    GLuint      vertex_binding  = 0;

    GLuint      texture_2d      = 0;
    GLuint      texture_3d      = 0;
    GLuint      texture_cube    = 0;

    GLuint      fb_all          = 0;
    GLuint      fb_draw         = 0;
    GLuint      fb_read         = 0;

    CVec4       clear_color;
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
