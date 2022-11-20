#pragma once

#ifdef GL_EXT_primitive_bounding_box
namespace gl::ext::primitive_bounding_box {
namespace values {
constexpr libc_types::u32 primitive_bounding_box = 0x92BE;
} // namespace values
/*!
 * \brief Part of GL_EXT_primitive_bounding_box
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
        GLW_FPTR_CHECK(PrimitiveBoundingBoxEXT)
    }
    glPrimitiveBoundingBoxEXT(minX, minY, minZ, minW, maxX, maxY, maxZ, maxW);
    detail::error_check("PrimitiveBoundingBoxEXT"sv);
}

} // namespace gl::ext::primitive_bounding_box
#endif // GL_EXT_primitive_bounding_box
namespace gl::ext::primitive_bounding_box {
constexpr auto name = "GL_EXT_primitive_bounding_box";
}
