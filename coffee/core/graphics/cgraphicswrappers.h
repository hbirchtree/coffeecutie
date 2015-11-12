#ifndef CGRAPHICSWRAPPERS_H
#define CGRAPHICSWRAPPERS_H

#include "cvertexarrayobject.h"
#include "cuniformtypes.h"

namespace Coffee{
namespace CGraphicsWrappers{

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

extern void coffee_graphics_enable_blend(bool enable, bool additive = false);
extern void coffee_graphics_enable_depth(bool enable);

extern void coffee_graphics_clear(CClearFlag flg);

extern void coffee_graphics_draw_indexed(
        const CPrimitiveMode& mode,
        const CGLDrawCall* drawcall);

extern void coffee_graphics_multidraw(
        const CPrimitiveMode& mode,
        const CBuffer* buf);

}
}

#endif // CGRAPHICSWRAPPERS_H
