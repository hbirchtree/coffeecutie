#ifndef CGRAPHICSWRAPPERS_H
#define CGRAPHICSWRAPPERS_H

#include "cvertexarrayobject.h"
#include "cuniformtypes.h"
#include "cbuffer.h"
#include "cframebuffer.h"
#include "crenderfence.h"
#include "cviewport_array.h"

namespace Coffee{
namespace CGraphicsWrappers{

/*!
 * \brief The default buffer to which graphics are rendered
 */
constexpr CGhnd opengl_default_framebuffer = 0;

/*!
 * \brief Set the GL viewport
 * \param view
 */
extern void coffee_graphics_set_viewport(const CRectF& view);

template<typename T>
/*!
 * \brief Convenience function for other kinds of sizes
 * \param view
 */
void coffee_graphics_set_viewport(
        const _cbasic_size_2d<T>& view)
{
    coffee_graphics_set_viewport(CRectF(0,0,view.w,view.h));
}
/*!
 * \brief Enable face culling
 * \param enable
 * \param mode
 */
extern void coffee_graphics_culling(bool enable, CCullMode mode);
/*!
 * \brief Enable blending
 * \param enable
 * \param additive
 */
extern void coffee_graphics_blend(bool enable, bool additive = false);
/*!
 * \brief Enable depth testing
 * \param enable
 */
extern void coffee_graphics_depth(bool enable);
/*!
 * \brief Set framebuffer clear-color
 * \param color
 */
extern void coffee_graphics_set_clearcolor(const CVec4& color);
/*!
 * \brief Clear framebuffer bits
 * \param flg
 */
extern void coffee_graphics_clear(CClearFlag flg);

extern void coffee_graphics_draw_indexed(
        const CPrimitiveMode& mode,
        const CGLDrawCall& drawcall);

extern void coffee_graphics_multidraw(
        const CPrimitiveMode& mode,
        const CBuffer &buf);
/*!
 * \brief Debug filter that ignores notifications.
 * \param rep
 * \return
 */
extern bool coffee_graphics_debug_filter_ignore_notifications(CGLDebugMessage const& rep);
/*!
 * \brief Debug filter that allows all messages
 * \return
 */
extern bool coffee_graphics_debug_filter_all(const CGLDebugMessage &);
/*!
 * \brief coffee_graphics_enable_debug_context
 * \param enable
 * \param userPointer
 * \return
 */
extern bool coffee_graphics_debug_context(
        bool enable,
        CDebugCallback userFunction = nullptr,
        void *userPtr = nullptr);

/*!
 * \brief Acquire a string containing all GL extensions
 * \return
 */
extern CString coffee_graphics_get_extensions(int32 *numExtensions_p = nullptr);

extern bool coffee_graphics_check_extension(cstring ext, cstring source);

}
}

#endif // CGRAPHICSWRAPPERS_H
