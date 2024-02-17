#pragma once

#ifdef GL_NV_pixel_data_range
#include "../enums/PixelDataRangeTargetNV.h"

namespace gl::nv::pixel_data_range {
using gl::group::pixel_data_range_target_nv;

namespace values {
constexpr u32 write_pixel_data_range_length  = 0x887A;
constexpr u32 read_pixel_data_range_length   = 0x887B;
constexpr u32 write_pixel_data_range_pointer = 0x887C;
constexpr u32 read_pixel_data_range_pointer  = 0x887D;
} // namespace values

/*!
 * \brief Part of GL_NV_pixel_data_range
 * \param target GLenum
 * \return void
 */
STATICINLINE void flush_pixel_data_range(
    group::pixel_data_range_target_nv target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FlushPixelDataRangeNV)
    }
    glFlushPixelDataRangeNV(static_cast<GLenum>(target));
    detail::error_check("FlushPixelDataRangeNV"sv);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_NV_pixel_data_range
 * \param target GLenum
 * \param length GLsizei
 * \param pointer const void *
 * \return void
 */
STATICINLINE void pixel_data_range(
    group::pixel_data_range_target_nv target, span_const_void const& pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PixelDataRangeNV)
    }
    glPixelDataRangeNV(
        static_cast<GLenum>(target),
        pointer.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("PixelDataRangeNV"sv);
}

} // namespace gl::nv::pixel_data_range
#endif // GL_NV_pixel_data_range
namespace gl::nv::pixel_data_range {
constexpr auto name = "GL_NV_pixel_data_range";
}
