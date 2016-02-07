#ifndef COFFEE_GRAPHICS_OPENGL_FENCE_H
#define COFFEE_GRAPHICS_OPENGL_FENCE_H

#include "copengl_enums.h"
#include <coffee/core/types/basetypes.h>

namespace Coffee{
namespace CGraphicsWrappers{

struct CGSync;

extern CGSync* coffee_graphics_fence_create();

extern void coffee_graphics_free(CGSync* snc);

extern bool coffee_graphics_fence_wait(CGSync *snc, uint64 timeout);

extern void coffee_graphics_memory_barrier(CMemoryBarrier bits);

}
}

#endif
