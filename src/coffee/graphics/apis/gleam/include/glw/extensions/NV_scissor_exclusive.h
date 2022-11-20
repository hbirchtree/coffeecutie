#pragma once

#ifdef GL_NV_scissor_exclusive
namespace gl::nv::scissor_exclusive {
namespace values {
constexpr libc_types::u32 scissor_test_exclusive = 0x9555;
constexpr libc_types::u32 scissor_box_exclusive  = 0x9556;
} // namespace values
template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_NV_scissor_exclusive
     * \param first GLuint
     * \param count GLsizei
     * \param v const GLint *
     * \return void
     */
    STATICINLINE
    void scissor_exclusive_arrayv(u32 first, i32 count, span_const_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ScissorExclusiveArrayvNV)
    }
    glScissorExclusiveArrayvNV(
        first,
        count,
        v.size() ? reinterpret_cast<const GLint*>(v.data()) : nullptr);
    detail::error_check("ScissorExclusiveArrayvNV"sv);
}

template<class size_2_i32, class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>&&
             semantic::concepts::Size2D<size_2_i32, i32>)
    /*!
     * \brief Part of GL_NV_scissor_exclusive
     * \param x GLint
     * \param y GLint
     * \param width GLsizei
     * \param height GLsizei
     * \return void
     */
    STATICINLINE
    void scissor_exclusive(vec_2_i32 const& x, size_2_i32 const& width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ScissorExclusiveNV)
    }
    glScissorExclusiveNV(x.x(), x.y(), width[0], width[1]);
    detail::error_check("ScissorExclusiveNV"sv);
}

} // namespace gl::nv::scissor_exclusive
#endif // GL_NV_scissor_exclusive
namespace gl::nv::scissor_exclusive {
constexpr auto name = "GL_NV_scissor_exclusive";
}
