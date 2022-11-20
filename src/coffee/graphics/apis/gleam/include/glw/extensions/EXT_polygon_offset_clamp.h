#pragma once

#ifdef GL_EXT_polygon_offset_clamp
namespace gl::ext::polygon_offset_clamp {
namespace values {
constexpr libc_types::u32 polygon_offset_clamp = 0x8E1B;
} // namespace values
/*!
 * \brief Part of GL_EXT_polygon_offset_clamp
 * \param factor GLfloat
 * \param units GLfloat
 * \param clamp GLfloat
 * \return void
 */
STATICINLINE void polygon_offset_clamp(f32 factor, f32 units, f32 clamp)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PolygonOffsetClampEXT)
    }
    glPolygonOffsetClampEXT(factor, units, clamp);
    detail::error_check("PolygonOffsetClampEXT"sv);
}

} // namespace gl::ext::polygon_offset_clamp
#endif // GL_EXT_polygon_offset_clamp
namespace gl::ext::polygon_offset_clamp {
constexpr auto name = "GL_EXT_polygon_offset_clamp";
}
