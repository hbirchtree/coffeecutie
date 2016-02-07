#ifndef COFFEE_GRAPHICS_APIS_OPENGL_WRAPPERS_COMPUTE_SHADER_H
#define COFFEE_GRAPHICS_APIS_OPENGL_WRAPPERS_COMPUTE_SHADER_H

#include "cbuffer.h"
#include "copengl_types.h"
#include "cshader.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct CComputeGroup
{
    CGuint num_x,num_y,num_z;
};

extern void coffee_graphics_compute_dispatch_direct(
        size_t count, const CComputeGroup* group);

inline void coffee_graphics_compute_dispatch_direct(const CComputeGroup& group)
{
    coffee_graphics_compute_dispatch_direct(1,&group);
}

extern void coffee_graphics_compute_dispatch(size_t count, const CSubBuffer *buffer);

inline void coffee_graphics_compute_dispatch(const CSubBuffer& group)
{
    coffee_graphics_compute_dispatch(1,&group);
}

}
}

#endif
