#pragma once

#ifdef GL_ARB_ES3_1_compatibility
#include "../enums/ColorBuffer.h"
#include "../enums/DrawBufferMode.h"
#include "../enums/ReadBufferMode.h"
#include "../enums/TriangleFace.h"

namespace gl::arb::es3_1_compatibility {
using gl::group::color_buffer;
using gl::group::draw_buffer_mode;
using gl::group::read_buffer_mode;
using gl::group::triangle_face;

namespace values {
} // namespace values

/*!
 * \brief Part of GL_ARB_ES3_1_compatibility
 * \param barriers GLbitfield
 * \return void
 */
STATICINLINE void memory_barrier_by_region(group::memory_barrier_mask barriers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MemoryBarrierByRegion)
    }
    glMemoryBarrierByRegion(static_cast<GLenum>(barriers));
    detail::error_check("MemoryBarrierByRegion"sv);
}

} // namespace gl::arb::es3_1_compatibility
#endif // GL_ARB_ES3_1_compatibility
namespace gl::arb::es3_1_compatibility {
constexpr auto name = "GL_ARB_ES3_1_compatibility";
}
