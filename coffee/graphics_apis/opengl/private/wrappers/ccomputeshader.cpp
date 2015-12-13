#include "ccomputeshader.h"

#include "glfunctions.h"

namespace Coffee{
namespace CGraphicsWrappers{

void coffee_graphics_compute_dispatch_direct(
        size_t count, const CComputeGroup *group)
{
    for(size_t i=0;i<count;i++)
    {
        glDispatchCompute(group[i].num_x,group[i].num_y,group[i].num_z);
    }
}

void coffee_graphics_compute_dispatch(
        size_t count, const CSubBuffer *buffer)
{
    for(size_t i=0;i<count;i++)
    {
        coffee_graphics_buffer_bind_range(buffer[i].parent,gl_get(buffer[i].type),
                                          buffer[i].offset,buffer[i].size,0);
        glDispatchComputeIndirect(nullptr);
    }
}


}
}
