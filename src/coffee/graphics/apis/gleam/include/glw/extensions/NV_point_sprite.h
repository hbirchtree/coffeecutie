#pragma once

#ifdef GL_NV_point_sprite
namespace gl::nv::point_sprite {
namespace values {
constexpr libc_types::u32 point_sprite        = 0x8861;
constexpr libc_types::u32 coord_replace       = 0x8862;
constexpr libc_types::u32 point_sprite_r_mode = 0x8863;
} // namespace values
STATICINLINE void point_parameter(
    group::point_parameter_name_arb pname, i32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PointParameteriNV)
    }
    glPointParameteriNV(static_cast<GLenum>(pname), param);
    detail::error_check("PointParameteriNV"sv);
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void point_parameter(
        group::point_parameter_name_arb pname, span_const_i32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PointParameterivNV)
    }
    glPointParameterivNV(
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("PointParameterivNV"sv);
}

} // namespace gl::nv::point_sprite
#endif // GL_NV_point_sprite
namespace gl::nv::point_sprite {
constexpr auto name = "GL_NV_point_sprite";
}
