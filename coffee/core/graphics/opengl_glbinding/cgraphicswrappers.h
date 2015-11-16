#ifndef CGRAPHICSWRAPPERS_H
#define CGRAPHICSWRAPPERS_H

#include "cvertexarrayobject.h"
#include "cuniformtypes.h"

namespace Coffee{
namespace CGraphicsWrappers{

/*!
 * \brief The default buffer to which graphics are rendered
 */
constexpr CGhnd opengl_default_framebuffer = 0;

/*!
 * \brief GL multidraw indirect call used to put triangles on the screen
 */
struct CGLDrawCall
{
    CGLDrawCall();

    CGuint count; /*!< Number of indices*/
    CGuint instanceCount; /*!< Number of instances*/
    CGuint firstIndex; /*!< First index in buffer*/
    CGuint baseVertex; /*!< First vertex in buffer*/
    CGuint baseInstance; /*!< First instance to render*/
};

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
