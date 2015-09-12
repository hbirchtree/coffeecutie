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
    GLuint count;
    GLuint instanceCount;
    GLuint firstIndex;
    GLuint baseVertex;
    GLuint baseInstance;
};

}
}

#endif // CGRAPHICSWRAPPERS_H
