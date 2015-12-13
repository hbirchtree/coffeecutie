#ifndef COFFEE_GRAPHICS_APIS_OPENGL_VIEWPORT_ARRAY_H
#define COFFEE_GRAPHICS_APIS_OPENGL_VIEWPORT_ARRAY_H

#include "copengl_types.h"
#include "coffee/graphics_apis/cgraphicsdata.h"

namespace Coffee{
namespace CGraphicsWrappers{

extern void coffee_graphics_viewport_set(
        const CGraphicsData::CViewport* view,
        const CGint& index,
        const CGint& numViewports);

extern void coffee_graphics_scissorview_set(
        const CGraphicsData::CScissorViewport* view,
        const CGint& index,
        const CGint& numViewports);

extern void coffee_graphics_depthview_set(
        const CGraphicsData::CScissorViewport* view,
        const CGint& index,
        const CGint& numViewports);

}
}

#endif
