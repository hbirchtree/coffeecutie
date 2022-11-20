#pragma once

#ifdef GL_ARB_ES3_2_compatibility
namespace gl::arb::es3_2_compatibility {
namespace values {
constexpr libc_types::u32 primitive_bounding_box             = 0x92BE;
constexpr libc_types::u32 multisample_line_width_range       = 0x9381;
constexpr libc_types::u32 multisample_line_width_granularity = 0x9382;
} // namespace values
/*!
 * \brief Part of GL_ARB_ES3_2_compatibility
 * \param minX GLfloat
 * \param minY GLfloat
 * \param minZ GLfloat
 * \param minW GLfloat
 * \param maxX GLfloat
 * \param maxY GLfloat
 * \param maxZ GLfloat
 * \param maxW GLfloat
 * \return void
 */
STATICINLINE void primitive_bounding_box(
    f32 minX,
    f32 minY,
    f32 minZ,
    f32 minW,
    f32 maxX,
    f32 maxY,
    f32 maxZ,
    f32 maxW)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PrimitiveBoundingBoxARB)
    }
    glPrimitiveBoundingBoxARB(minX, minY, minZ, minW, maxX, maxY, maxZ, maxW);
    detail::error_check("PrimitiveBoundingBoxARB"sv);
}

} // namespace gl::arb::es3_2_compatibility
#endif // GL_ARB_ES3_2_compatibility
namespace gl::arb::es3_2_compatibility {
constexpr auto name = "GL_ARB_ES3_2_compatibility";
}
