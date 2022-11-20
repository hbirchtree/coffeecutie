#ifdef GL_VERSION_2_1
template<class span_const_mat_2x3_f32>
requires(
    MinimumVersion<Current, Version<2, 1>>&&
        semantic::concepts::Span<span_const_mat_2x3_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_2x3_f32::value_type, f32, 2, 3>)
    /*!
     * \brief Wraps around glUniformMatrix2x3fv. Introduced in GL core 2.1
     * \param location GLint
     * \param count GLsizei
     * \param transpose GLboolean
     * \param value const GLfloat *
     * \return void
     */
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_2x3_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix2x3fv)
    }
    glUniformMatrix2x3fv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix2x3fv"sv);
}

template<class span_const_mat_2x4_f32>
requires(
    MinimumVersion<Current, Version<2, 1>>&&
        semantic::concepts::Span<span_const_mat_2x4_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_2x4_f32::value_type, f32, 2, 4>)
    /*!
     * \brief Wraps around glUniformMatrix2x4fv. Introduced in GL core 2.1
     * \param location GLint
     * \param count GLsizei
     * \param transpose GLboolean
     * \param value const GLfloat *
     * \return void
     */
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_2x4_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix2x4fv)
    }
    glUniformMatrix2x4fv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix2x4fv"sv);
}

template<class span_const_mat_3x2_f32>
requires(
    MinimumVersion<Current, Version<2, 1>>&&
        semantic::concepts::Span<span_const_mat_3x2_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_3x2_f32::value_type, f32, 3, 2>)
    /*!
     * \brief Wraps around glUniformMatrix3x2fv. Introduced in GL core 2.1
     * \param location GLint
     * \param count GLsizei
     * \param transpose GLboolean
     * \param value const GLfloat *
     * \return void
     */
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_3x2_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix3x2fv)
    }
    glUniformMatrix3x2fv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix3x2fv"sv);
}

template<class span_const_mat_3x4_f32>
requires(
    MinimumVersion<Current, Version<2, 1>>&&
        semantic::concepts::Span<span_const_mat_3x4_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_3x4_f32::value_type, f32, 3, 4>)
    /*!
     * \brief Wraps around glUniformMatrix3x4fv. Introduced in GL core 2.1
     * \param location GLint
     * \param count GLsizei
     * \param transpose GLboolean
     * \param value const GLfloat *
     * \return void
     */
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_3x4_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix3x4fv)
    }
    glUniformMatrix3x4fv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix3x4fv"sv);
}

template<class span_const_mat_4x2_f32>
requires(
    MinimumVersion<Current, Version<2, 1>>&&
        semantic::concepts::Span<span_const_mat_4x2_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_4x2_f32::value_type, f32, 4, 2>)
    /*!
     * \brief Wraps around glUniformMatrix4x2fv. Introduced in GL core 2.1
     * \param location GLint
     * \param count GLsizei
     * \param transpose GLboolean
     * \param value const GLfloat *
     * \return void
     */
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_4x2_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix4x2fv)
    }
    glUniformMatrix4x2fv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix4x2fv"sv);
}

template<class span_const_mat_4x3_f32>
requires(
    MinimumVersion<Current, Version<2, 1>>&&
        semantic::concepts::Span<span_const_mat_4x3_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_4x3_f32::value_type, f32, 4, 3>)
    /*!
     * \brief Wraps around glUniformMatrix4x3fv. Introduced in GL core 2.1
     * \param location GLint
     * \param count GLsizei
     * \param transpose GLboolean
     * \param value const GLfloat *
     * \return void
     */
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_4x3_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix4x3fv)
    }
    glUniformMatrix4x3fv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix4x3fv"sv);
}

#endif // GL_VERSION_2_1
