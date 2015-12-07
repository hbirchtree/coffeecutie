#ifndef CGRAPHICSWRAPPERS_H
#define CGRAPHICSWRAPPERS_H

#include "cvertexarrayobject.h"
#include "cuniformtypes.h"
#include "cbuffer.h"
#include "cframebuffer.h"
#include "crenderfence.h"

namespace Coffee{
namespace CGraphicsWrappers{

enum class CPrimitiveMode : uint16
{
    Triangles = 0,
    Points = 1,
    Line = 2,
    TriangleStrip = 3,
    TriangleFan = 4,
    TriangleStripAdjacency = 5,
    TrianglesAdjacency = 6,
    LineStrip = 7,
    LineLoop = 8,
};

enum class CCullMode : uint8
{
    Front = 1,
    Back  = 2,
    Both  = 3,
};

/*!
 * \brief The default buffer to which graphics are rendered
 */
constexpr CGhnd opengl_default_framebuffer = 0;

extern void coffee_graphics_set_viewport(const CRectF& view);

template<typename T>
void coffee_graphics_set_viewport(
        const _cbasic_size<T>& view)
{
    coffee_graphics_set_viewport(CRectF(0,0,view.w,view.h));
}

extern void coffee_graphics_culling(bool enable, CCullMode mode);

extern void coffee_graphics_blend(bool enable, bool additive = false);

extern void coffee_graphics_depth(bool enable);

extern void coffee_graphics_set_clearcolor(const CVec4& color);

extern void coffee_graphics_clear(CClearFlag flg);

extern void coffee_graphics_draw_indexed(
        const CPrimitiveMode& mode,
        const CGLDrawCall* drawcall);

extern void coffee_graphics_multidraw(
        const CPrimitiveMode& mode,
        const CBuffer* buf);

extern bool coffee_graphics_debug_filter_ignore_notifications(CGLReport* rep);

extern bool coffee_graphics_debug_filter_all(CGLReport*);

}
}

#endif // CGRAPHICSWRAPPERS_H
