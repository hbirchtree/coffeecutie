#pragma once

#ifdef GL_AMD_sample_positions
namespace gl::amd::sample_positions {
namespace values {
constexpr u32 subsample_distance = 0x883F;
} // namespace values

template<class span_const_f32>
requires(
    concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_AMD_sample_positions
 * \param pname GLenum
 * \param index GLuint
 * \param val const GLfloat *
 * \return void
 */
STATICINLINE void set_multisamplefv(
    GLenum                pname,
    u32                   index,
    span_const_f32 const& val,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SetMultisamplefvAMD)
    }
    glSetMultisamplefvAMD(
        pname,
        index,
        val.size() ? reinterpret_cast<const GLfloat*>(val.data()) : nullptr);
    detail::error_check("SetMultisamplefvAMD"sv, check_errors);
}

} // namespace gl::amd::sample_positions
#endif // GL_AMD_sample_positions
namespace gl::amd::sample_positions {
constexpr auto name = "GL_AMD_sample_positions";
}
