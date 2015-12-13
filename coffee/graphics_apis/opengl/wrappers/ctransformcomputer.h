#ifndef COFFEE_GRAPHICS_APIS_OPENGL_CTRANSFORMCOMPUTER_H
#define COFFEE_GRAPHICS_APIS_OPENGL_CTRANSFORMCOMPUTER_H

#include "copengl_types.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct CXFBuffer : _cbasic_graphics_resource {};

struct CXFBufferOutput
{
    size_t numVariables;
    cstring *variables;
};

extern void coffee_graphics_alloc(size_t count, CXFBuffer* obj);

inline void coffee_graphics_alloc(CXFBuffer& obj)
{
    coffee_graphics_alloc(1,&obj);
}

extern void coffee_graphics_free(size_t count, CXFBuffer* obj);

inline void coffee_graphics_free(CXFBuffer& obj)
{
    coffee_graphics_free(1,&obj);
}

extern void coffee_graphics_bind(CXFBuffer& obj);
extern void coffee_graphics_unbind(CXFBuffer&);

extern void coffee_graphics_xfb_draw(
        CXFBuffer& obj,
        const CPrimitiveMode& mode,
        const CGuint& stream,
        const CGsize& instances);

extern void coffee_graphics_xfb_begin(const CPrimitiveMode& mode);

extern void coffee_graphics_xfb_pause();
extern void coffee_graphics_xfb_resume();

extern void coffee_graphics_xfb_end();

extern void coffee_graphics_xfb_set_output(
        _cbasic_graphics_shader_program& shader,
        const CXFBufferOutput& output,
        const CAttributeMode& mode);

}
}

#endif
