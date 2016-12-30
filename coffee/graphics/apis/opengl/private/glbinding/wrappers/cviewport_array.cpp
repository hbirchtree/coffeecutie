#include <coffee/graphics_apis/opengl/glbinding/wrappers/cviewport_array.h>
#include <coffee/graphics_apis/opengl/glfunctions.h>

namespace Coffee{
namespace CGraphicsWrappers{

void coffee_graphics_viewport_set(
        const CGraphicsData::CViewport *view,
        const CGint &index, const CGint &numViewports)
{
    glViewportArrayv(index,numViewports,(scalar*)view);
}

void coffee_graphics_scissorview_set(
        const CGraphicsData::CScissorViewport *view,
        const CGint &index, const CGint &numViewports)
{
    glScissorArrayv(index,numViewports,(CGint*)view);
}

void coffee_graphics_depthview_set(
        const CGraphicsData::CDepthViewport *view,
        const CGint &index, const CGint &numViewports)
{
    glDepthRangeArrayv(index,numViewports,(bigscalar*)view);
}

}
}
