#ifdef GL_VERSION_1_4
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 4>>)
/*!
 * \brief Wraps around glBlendFuncSeparate. Introduced in GL core 1.4
 * \param sfactorRGB GLenum
 * \param dfactorRGB GLenum
 * \param sfactorAlpha GLenum
 * \param dfactorAlpha GLenum
 * \return void
 */
STATICINLINE void blend_func_separate(
    group::blending_factor sfactorRGB,
    group::blending_factor dfactorRGB,
    group::blending_factor sfactorAlpha,
    group::blending_factor dfactorAlpha,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(BlendFuncSeparate)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glBlendFuncSeparate,
        sfactorRGB,
        dfactorRGB,
        sfactorAlpha,
        dfactorAlpha);
#endif
    glBlendFuncSeparate(
        static_cast<GLenum>(sfactorRGB),
        static_cast<GLenum>(dfactorRGB),
        static_cast<GLenum>(sfactorAlpha),
        static_cast<GLenum>(dfactorAlpha));
    detail::error_check("BlendFuncSeparate"sv, check_errors);
}

template<class span_const_i32>
requires(
    MinimumVersion<Current, Version<1, 4>> && concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glMultiDrawArrays. Introduced in GL core 1.4
 * \param mode GLenum
 * \param first const GLint *
 * \param count const GLsizei *
 * \param drawcount GLsizei
 * \return void
 */
STATICINLINE void multi_draw_arrays(
    group::primitive_type mode,
    span_const_i32        first,
    span_const_i32        count,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(MultiDrawArrays)
    }
    GLsizei drawcount = first.size();
    detail::assert_equal(first.size(), drawcount);
    detail::assert_equal(count.size(), drawcount);
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glMultiDrawArrays,
        mode,
        gsl::span<const char>(
            reinterpret_cast<const char*>(first.data()), first.size_bytes()),
        gsl::span<const char>(
            reinterpret_cast<const char*>(count.data()), count.size_bytes()));
#endif
    glMultiDrawArrays(static_cast<GLenum>(mode), first.data(), count.data());
    detail::error_check("MultiDrawArrays"sv, check_errors);
}

template<class span_const_i32>
requires(
    MinimumVersion<Current, Version<1, 4>> && concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glMultiDrawElements. Introduced in GL core 1.4
 * \param mode GLenum
 * \param count const GLsizei *
 * \param type GLenum
 * \param indices const void *const*
 * \param drawcount GLsizei
 * \return void
 */
STATICINLINE void multi_draw_elements(
    group::primitive_type     mode,
    span_const_i32            count,
    group::draw_elements_type type,
    intptr_t                  indices,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(MultiDrawElements)
    }
    GLsizei drawcount = count.size();
    detail::assert_equal(count.size(), drawcount);
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glMultiDrawElements,
        gsl::span<const char>(
            reinterpret_cast<const char*>(count.data()), count.size_bytes()),
        mode,
        type,
        indices);
#endif
    glMultiDrawElements(
        static_cast<GLenum>(mode),
        count.data(),
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices));
    detail::error_check("MultiDrawElements"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 4>>)
/*!
 * \brief Wraps around glPointParameterf. Introduced in GL core 1.4
 * \param pname GLenum
 * \param param GLfloat
 * \return void
 */
STATICINLINE void point_parameter(
    group::point_parameter_name_arb pname,
    f32                             param,
    error_check                     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(PointParameterf)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glPointParameterf, pname, param);
#endif
    glPointParameterf(static_cast<GLenum>(pname), param);
    detail::error_check("PointParameterf"sv, check_errors);
}

template<class span_const_f32>
requires(
    MinimumVersion<Current, Version<1, 4>> && concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Wraps around glPointParameterfv. Introduced in GL core 1.4
 * \param pname GLenum
 * \param params const GLfloat *
 * \return void
 */
STATICINLINE void point_parameter(
    group::point_parameter_name_arb pname,
    span_const_f32 const&           params,
    error_check                     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(PointParameterfv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glPointParameterfv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(params.data()), params.size_bytes()),
        pname);
#endif
    glPointParameterfv(
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfloat*>(params.data())
                      : nullptr);
    detail::error_check("PointParameterfv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 4>>)
/*!
 * \brief Wraps around glPointParameteri. Introduced in GL core 1.4
 * \param pname GLenum
 * \param param GLint
 * \return void
 */
STATICINLINE void point_parameter(
    group::point_parameter_name_arb pname,
    i32                             param,
    error_check                     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(PointParameteri)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glPointParameteri, pname, param);
#endif
    glPointParameteri(static_cast<GLenum>(pname), param);
    detail::error_check("PointParameteri"sv, check_errors);
}

template<class span_const_i32>
requires(
    MinimumVersion<Current, Version<1, 4>> && concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glPointParameteriv. Introduced in GL core 1.4
 * \param pname GLenum
 * \param params const GLint *
 * \return void
 */
STATICINLINE void point_parameter(
    group::point_parameter_name_arb pname,
    span_const_i32 const&           params,
    error_check                     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(PointParameteriv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glPointParameteriv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(params.data()), params.size_bytes()),
        pname);
#endif
    glPointParameteriv(
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("PointParameteriv"sv, check_errors);
}

template<class vec_4_f32>
requires(
    MinimumVersion<Current, Version<1, 4>> &&
    concepts::vector<vec_4_f32, f32, 4>)
/*!
 * \brief Wraps around glBlendColor. Introduced in GL core 1.4
 * \param red GLfloat
 * \param green GLfloat
 * \param blue GLfloat
 * \param alpha GLfloat
 * \return void
 */
STATICINLINE void blend_color(
    vec_4_f32 const& red, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(BlendColor)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBlendColor, red);
#endif
    glBlendColor(red[0], red[1], red[2], red[3]);
    detail::error_check("BlendColor"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 4>>)
/*!
 * \brief Wraps around glBlendEquation. Introduced in GL core 1.4
 * \param mode GLenum
 * \return void
 */
STATICINLINE void blend_equation(
    group::blend_equation_mode_ext mode,
    error_check                    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(BlendEquation)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBlendEquation, mode);
#endif
    glBlendEquation(static_cast<GLenum>(mode));
    detail::error_check("BlendEquation"sv, check_errors);
}

#endif // GL_VERSION_1_4
