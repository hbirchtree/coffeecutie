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
        const CGLDrawCall* drawcall);

extern void coffee_graphics_multidraw(
        const CPrimitiveMode& mode,
        const CBuffer* buf);
/*!
 * \brief Debug filter that ignores notifications.
 * \param rep
 * \return
 */
extern bool coffee_graphics_debug_filter_ignore_notifications(CGLReport* rep);
/*!
 * \brief Debug filter that ignores all messages. It's better to disable debugging, but...
 * \return
 */
extern bool coffee_graphics_debug_filter_all(CGLReport*);
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
 * \brief Translates a CGLReport* into a user-readable representation. CGLReport should not be used outside of Core.
 * \param report
 * \return
 */
extern CGLDebugMessage coffee_graphics_debug_report(CGLReport const* report);

/*!
 * \brief Acquire a string containing all GL extensions
 * \return
 */
extern CString coffee_graphics_get_extensions(int32 *numExtensions_p = nullptr);

extern bool coffee_graphics_check_extension(cstring ext, cstring source);

}
}

#endif // CGRAPHICSWRAPPERS_H
