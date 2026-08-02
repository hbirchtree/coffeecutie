#ifdef GL_VERSION_4_0
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>)
/*!
 * \brief Wraps around glBlendEquationSeparatei. Introduced in GL core 4.0
 * \param buf GLuint
 * \param modeRGB GLenum
 * \param modeAlpha GLenum
 * \return void
 */
STATICINLINE void blend_equation_separatei(
    u32                            buf,
    group::blend_equation_mode_ext modeRGB,
    group::blend_equation_mode_ext modeAlpha,
    error_check                    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(BlendEquationSeparatei)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBlendEquationSeparatei, buf, modeRGB, modeAlpha);
#endif
    glBlendEquationSeparatei(
        buf, static_cast<GLenum>(modeRGB), static_cast<GLenum>(modeAlpha));
    detail::error_check("BlendEquationSeparatei"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>)
/*!
 * \brief Wraps around glBlendEquationi. Introduced in GL core 4.0
 * \param buf GLuint
 * \param mode GLenum
 * \return void
 */
STATICINLINE void blend_equationi(
    u32                            buf,
    group::blend_equation_mode_ext mode,
    error_check                    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(BlendEquationi)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBlendEquationi, buf, mode);
#endif
    glBlendEquationi(buf, static_cast<GLenum>(mode));
    detail::error_check("BlendEquationi"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>)
/*!
 * \brief Wraps around glBlendFuncSeparatei. Introduced in GL core 4.0
 * \param buf GLuint
 * \param srcRGB GLenum
 * \param dstRGB GLenum
 * \param srcAlpha GLenum
 * \param dstAlpha GLenum
 * \return void
 */
STATICINLINE void blend_func_separatei(
    u32                    buf,
    group::blending_factor srcRGB,
    group::blending_factor dstRGB,
    group::blending_factor srcAlpha,
    group::blending_factor dstAlpha,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(BlendFuncSeparatei)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glBlendFuncSeparatei, buf, srcRGB, dstRGB, srcAlpha, dstAlpha);
#endif
    glBlendFuncSeparatei(
        buf,
        static_cast<GLenum>(srcRGB),
        static_cast<GLenum>(dstRGB),
        static_cast<GLenum>(srcAlpha),
        static_cast<GLenum>(dstAlpha));
    detail::error_check("BlendFuncSeparatei"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>)
/*!
 * \brief Wraps around glBlendFunci. Introduced in GL core 4.0
 * \param buf GLuint
 * \param src GLenum
 * \param dst GLenum
 * \return void
 */
STATICINLINE void blend_funci(
    u32                    buf,
    group::blending_factor src,
    group::blending_factor dst,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(BlendFunci)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBlendFunci, buf, src, dst);
#endif
    glBlendFunci(buf, static_cast<GLenum>(src), static_cast<GLenum>(dst));
    detail::error_check("BlendFunci"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>)
/*!
 * \brief Wraps around glMinSampleShading. Introduced in GL core 4.0
 * \param value GLfloat
 * \return void
 */
STATICINLINE void min_sample_shading(
    f32 value, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(MinSampleShading)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMinSampleShading, value);
#endif
    glMinSampleShading(value);
    detail::error_check("MinSampleShading"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>)
/*!
 * \brief Wraps around glDrawArraysIndirect. Introduced in GL core 4.0
 * \param mode GLenum
 * \param indirect const void *
 * \return void
 */
STATICINLINE void draw_arrays_indirect(
    group::primitive_type mode,
    intptr_t              indirect,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(DrawArraysIndirect)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glDrawArraysIndirect, mode, indirect);
#endif
    glDrawArraysIndirect(
        static_cast<GLenum>(mode), reinterpret_cast<const void*>(indirect));
    detail::error_check("DrawArraysIndirect"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>)
/*!
 * \brief Wraps around glDrawElementsIndirect. Introduced in GL core 4.0
 * \param mode GLenum
 * \param type GLenum
 * \param indirect const void *
 * \return void
 */
STATICINLINE void draw_elements_indirect(
    group::primitive_type     mode,
    group::draw_elements_type type,
    intptr_t                  indirect,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(DrawElementsIndirect)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glDrawElementsIndirect, mode, type, indirect);
#endif
    glDrawElementsIndirect(
        static_cast<GLenum>(mode),
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indirect));
    detail::error_check("DrawElementsIndirect"sv, check_errors);
}

template<class span_f64>
requires(
    MinimumVersion<Current, Version<4, 0>> && concepts::span<span_f64> &&
    std::is_same_v<
        std::decay_t<typename span_f64::value_type>,
        std::decay_t<f64>>)
/*!
 * \brief Wraps around glGetUniformdv. Introduced in GL core 4.0
 * \param program GLuint
 * \param location GLint
 * \param params GLdouble *
 * \return void
 */
STATICINLINE void get_uniformdv(
    u32         program,
    i32         location,
    span_f64    params,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetUniformdv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetUniformdv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        program,
        location);
#endif
    glGetUniformdv(
        program,
        location,
        params.size() ? reinterpret_cast<GLdouble*>(params.data()) : nullptr);
    detail::error_check("GetUniformdv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>)
/*!
 * \brief Wraps around glUniform1d. Introduced in GL core 4.0
 * \param location GLint
 * \param x GLdouble
 * \return void
 */
STATICINLINE void uniform(
    i32 location, f64 x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(Uniform1d)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glUniform1d, location, x);
#endif
    glUniform1d(location, x);
    detail::error_check("Uniform1d"sv, check_errors);
}

template<class span_const_f64>
requires(
    MinimumVersion<Current, Version<4, 0>> && concepts::span<span_const_f64> &&
    std::is_same_v<
        std::decay_t<typename span_const_f64::value_type>,
        std::decay_t<f64>>)
/*!
 * \brief Wraps around glUniform1dv. Introduced in GL core 4.0
 * \param location GLint
 * \param count GLsizei
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(
    i32                   location,
    span_const_f64 const& value,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(Uniform1dv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniform1dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location);
#endif
    glUniform1dv(
        location,
        value.size(),
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("Uniform1dv"sv, check_errors);
}

template<class vec_2_f64>
requires(
    MinimumVersion<Current, Version<4, 0>> &&
    concepts::vector<vec_2_f64, f64, 2>)
/*!
 * \brief Wraps around glUniform2d. Introduced in GL core 4.0
 * \param location GLint
 * \param x GLdouble
 * \param y GLdouble
 * \return void
 */
STATICINLINE void uniform(
    i32              location,
    vec_2_f64 const& x,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(Uniform2d)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glUniform2d, location, x);
#endif
    glUniform2d(location, x[0], x[1]);
    detail::error_check("Uniform2d"sv, check_errors);
}

template<class span_const_vec_2_f64>
requires(
    MinimumVersion<Current, Version<4, 0>> &&
    concepts::span<span_const_vec_2_f64> &&
    concepts::vector<typename span_const_vec_2_f64::value_type, f64, 2>)
/*!
 * \brief Wraps around glUniform2dv. Introduced in GL core 4.0
 * \param location GLint
 * \param count GLsizei
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(
    i32                         location,
    span_const_vec_2_f64 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(Uniform2dv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniform2dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location);
#endif
    glUniform2dv(
        location,
        value.size(),
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("Uniform2dv"sv, check_errors);
}

template<class vec_3_f64>
requires(
    MinimumVersion<Current, Version<4, 0>> &&
    concepts::vector<vec_3_f64, f64, 3>)
/*!
 * \brief Wraps around glUniform3d. Introduced in GL core 4.0
 * \param location GLint
 * \param x GLdouble
 * \param y GLdouble
 * \param z GLdouble
 * \return void
 */
STATICINLINE void uniform(
    i32              location,
    vec_3_f64 const& x,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(Uniform3d)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glUniform3d, location, x);
#endif
    glUniform3d(location, x[0], x[1], x[2]);
    detail::error_check("Uniform3d"sv, check_errors);
}

template<class span_const_vec_3_f64>
requires(
    MinimumVersion<Current, Version<4, 0>> &&
    concepts::span<span_const_vec_3_f64> &&
    concepts::vector<typename span_const_vec_3_f64::value_type, f64, 3>)
/*!
 * \brief Wraps around glUniform3dv. Introduced in GL core 4.0
 * \param location GLint
 * \param count GLsizei
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(
    i32                         location,
    span_const_vec_3_f64 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(Uniform3dv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniform3dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location);
#endif
    glUniform3dv(
        location,
        value.size(),
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("Uniform3dv"sv, check_errors);
}

template<class vec_4_f64>
requires(
    MinimumVersion<Current, Version<4, 0>> &&
    concepts::vector<vec_4_f64, f64, 4>)
/*!
 * \brief Wraps around glUniform4d. Introduced in GL core 4.0
 * \param location GLint
 * \param x GLdouble
 * \param y GLdouble
 * \param z GLdouble
 * \param w GLdouble
 * \return void
 */
STATICINLINE void uniform(
    i32              location,
    vec_4_f64 const& x,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(Uniform4d)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glUniform4d, location, x);
#endif
    glUniform4d(location, x[0], x[1], x[2], x[3]);
    detail::error_check("Uniform4d"sv, check_errors);
}

template<class span_const_vec_4_f64>
requires(
    MinimumVersion<Current, Version<4, 0>> &&
    concepts::span<span_const_vec_4_f64> &&
    concepts::vector<typename span_const_vec_4_f64::value_type, f64, 4>)
/*!
 * \brief Wraps around glUniform4dv. Introduced in GL core 4.0
 * \param location GLint
 * \param count GLsizei
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(
    i32                         location,
    span_const_vec_4_f64 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(Uniform4dv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniform4dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location);
#endif
    glUniform4dv(
        location,
        value.size(),
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("Uniform4dv"sv, check_errors);
}

template<class span_const_mat_2x2_f64>
requires(
    MinimumVersion<Current, Version<4, 0>> &&
    concepts::span<span_const_mat_2x2_f64> &&
    concepts::matrix<typename span_const_mat_2x2_f64::value_type, f64, 2, 2>)
/*!
 * \brief Wraps around glUniformMatrix2dv. Introduced in GL core 4.0
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_2x2_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(UniformMatrix2dv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix2dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix2dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix2dv"sv, check_errors);
}

template<class span_const_mat_2x3_f64>
requires(
    MinimumVersion<Current, Version<4, 0>> &&
    concepts::span<span_const_mat_2x3_f64> &&
    concepts::matrix<typename span_const_mat_2x3_f64::value_type, f64, 2, 3>)
/*!
 * \brief Wraps around glUniformMatrix2x3dv. Introduced in GL core 4.0
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_2x3_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(UniformMatrix2x3dv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix2x3dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix2x3dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix2x3dv"sv, check_errors);
}

template<class span_const_mat_2x4_f64>
requires(
    MinimumVersion<Current, Version<4, 0>> &&
    concepts::span<span_const_mat_2x4_f64> &&
    concepts::matrix<typename span_const_mat_2x4_f64::value_type, f64, 2, 4>)
/*!
 * \brief Wraps around glUniformMatrix2x4dv. Introduced in GL core 4.0
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_2x4_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(UniformMatrix2x4dv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix2x4dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix2x4dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix2x4dv"sv, check_errors);
}

template<class span_const_mat_3x3_f64>
requires(
    MinimumVersion<Current, Version<4, 0>> &&
    concepts::span<span_const_mat_3x3_f64> &&
    concepts::matrix<typename span_const_mat_3x3_f64::value_type, f64, 3, 3>)
/*!
 * \brief Wraps around glUniformMatrix3dv. Introduced in GL core 4.0
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_3x3_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(UniformMatrix3dv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix3dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix3dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix3dv"sv, check_errors);
}

template<class span_const_mat_3x2_f64>
requires(
    MinimumVersion<Current, Version<4, 0>> &&
    concepts::span<span_const_mat_3x2_f64> &&
    concepts::matrix<typename span_const_mat_3x2_f64::value_type, f64, 3, 2>)
/*!
 * \brief Wraps around glUniformMatrix3x2dv. Introduced in GL core 4.0
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_3x2_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(UniformMatrix3x2dv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix3x2dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix3x2dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix3x2dv"sv, check_errors);
}

template<class span_const_mat_3x4_f64>
requires(
    MinimumVersion<Current, Version<4, 0>> &&
    concepts::span<span_const_mat_3x4_f64> &&
    concepts::matrix<typename span_const_mat_3x4_f64::value_type, f64, 3, 4>)
/*!
 * \brief Wraps around glUniformMatrix3x4dv. Introduced in GL core 4.0
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_3x4_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(UniformMatrix3x4dv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix3x4dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix3x4dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix3x4dv"sv, check_errors);
}

template<class span_const_mat_4x4_f64>
requires(
    MinimumVersion<Current, Version<4, 0>> &&
    concepts::span<span_const_mat_4x4_f64> &&
    concepts::matrix<typename span_const_mat_4x4_f64::value_type, f64, 4, 4>)
/*!
 * \brief Wraps around glUniformMatrix4dv. Introduced in GL core 4.0
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_4x4_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(UniformMatrix4dv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix4dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix4dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix4dv"sv, check_errors);
}

template<class span_const_mat_4x2_f64>
requires(
    MinimumVersion<Current, Version<4, 0>> &&
    concepts::span<span_const_mat_4x2_f64> &&
    concepts::matrix<typename span_const_mat_4x2_f64::value_type, f64, 4, 2>)
/*!
 * \brief Wraps around glUniformMatrix4x2dv. Introduced in GL core 4.0
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_4x2_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(UniformMatrix4x2dv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix4x2dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix4x2dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix4x2dv"sv, check_errors);
}

template<class span_const_mat_4x3_f64>
requires(
    MinimumVersion<Current, Version<4, 0>> &&
    concepts::span<span_const_mat_4x3_f64> &&
    concepts::matrix<typename span_const_mat_4x3_f64::value_type, f64, 4, 3>)
/*!
 * \brief Wraps around glUniformMatrix4x3dv. Introduced in GL core 4.0
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_4x3_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(UniformMatrix4x3dv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix4x3dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix4x3dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix4x3dv"sv, check_errors);
}

template<class span_GLchar>
requires(
    MinimumVersion<Current, Version<4, 0>> && concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>>)
/*!
 * \brief Wraps around glGetActiveSubroutineName. Introduced in GL core 4.0
 * \param program GLuint
 * \param shadertype GLenum
 * \param index GLuint
 * \param bufSize GLsizei
 * \param length GLsizei *
 * \param name GLchar *
 * \return void
 */
STATICINLINE void get_active_subroutine_name(
    u32                program,
    group::shader_type shadertype,
    u32                index,
    i32&               length,
    span_GLchar        name,
    error_check        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetActiveSubroutineName)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetActiveSubroutineName,
        gsl::span<char>(
            reinterpret_cast<char*>(name.data()), name.size_bytes()),
        program,
        shadertype,
        index,
        length);
#endif
    glGetActiveSubroutineName(
        program,
        static_cast<GLenum>(shadertype),
        index,
        name.size(),
        &length,
        name.data());
    detail::error_check("GetActiveSubroutineName"sv, check_errors);
}

template<class span_GLchar>
requires(
    MinimumVersion<Current, Version<4, 0>> && concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>>)
/*!
 * \brief Wraps around glGetActiveSubroutineUniformName. Introduced in GL
 * core 4.0
 * \param program GLuint
 * \param shadertype GLenum
 * \param index GLuint
 * \param bufSize GLsizei
 * \param length GLsizei *
 * \param name GLchar *
 * \return void
 */
STATICINLINE void get_active_subroutine_uniform_name(
    u32                program,
    group::shader_type shadertype,
    u32                index,
    i32&               length,
    span_GLchar        name,
    error_check        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetActiveSubroutineUniformName)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetActiveSubroutineUniformName,
        gsl::span<char>(
            reinterpret_cast<char*>(name.data()), name.size_bytes()),
        program,
        shadertype,
        index,
        length);
#endif
    glGetActiveSubroutineUniformName(
        program,
        static_cast<GLenum>(shadertype),
        index,
        name.size(),
        &length,
        name.data());
    detail::error_check("GetActiveSubroutineUniformName"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<4, 0>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetActiveSubroutineUniformiv. Introduced in GL core 4.0
 * \param program GLuint
 * \param shadertype GLenum
 * \param index GLuint
 * \param pname GLenum
 * \param values GLint *
 * \return void
 */
STATICINLINE void get_active_subroutine_uniformiv(
    u32                              program,
    group::shader_type               shadertype,
    u32                              index,
    group::subroutine_parameter_name pname,
    span_i32                         values,
    error_check                      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetActiveSubroutineUniformiv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetActiveSubroutineUniformiv,
        gsl::span<char>(
            reinterpret_cast<char*>(values.data()), values.size_bytes()),
        program,
        shadertype,
        index,
        pname);
#endif
    glGetActiveSubroutineUniformiv(
        program,
        static_cast<GLenum>(shadertype),
        index,
        static_cast<GLenum>(pname),
        values.size() ? reinterpret_cast<GLint*>(values.data()) : nullptr);
    detail::error_check("GetActiveSubroutineUniformiv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>)
/*!
 * \brief Wraps around glGetProgramStageiv. Introduced in GL core 4.0
 * \param program GLuint
 * \param shadertype GLenum
 * \param pname GLenum
 * \param values GLint *
 * \return void
 */
STATICINLINE void get_program_stageiv(
    u32                       program,
    group::shader_type        shadertype,
    group::program_stage_prop pname,
    i32&                      values,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetProgramStageiv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glGetProgramStageiv, program, shadertype, pname, values);
#endif
    glGetProgramStageiv(
        program,
        static_cast<GLenum>(shadertype),
        static_cast<GLenum>(pname),
        &values);
    detail::error_check("GetProgramStageiv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>)
/*!
 * \brief Wraps around glGetSubroutineIndex. Introduced in GL core 4.0
 * \param program GLuint
 * \param shadertype GLenum
 * \param name const GLchar *
 * \return GLuint
 */
STATICINLINE GLuint get_subroutine_index(
    u32                     program,
    group::shader_type      shadertype,
    std::string_view const& name,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetSubroutineIndex)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glGetSubroutineIndex, program, shadertype, name);
#endif
    auto out = glGetSubroutineIndex(
        program, static_cast<GLenum>(shadertype), name.data());
    detail::error_check("GetSubroutineIndex"sv, check_errors);
    return out;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>)
/*!
 * \brief Wraps around glGetSubroutineUniformLocation. Introduced in GL core 4.0
 * \param program GLuint
 * \param shadertype GLenum
 * \param name const GLchar *
 * \return GLint
 */
STATICINLINE GLint get_subroutine_uniform_location(
    u32                     program,
    group::shader_type      shadertype,
    std::string_view const& name,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetSubroutineUniformLocation)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glGetSubroutineUniformLocation, program, shadertype, name);
#endif
    auto out = glGetSubroutineUniformLocation(
        program, static_cast<GLenum>(shadertype), name.data());
    detail::error_check("GetSubroutineUniformLocation"sv, check_errors);
    return out;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>)
/*!
 * \brief Wraps around glGetUniformSubroutineuiv. Introduced in GL core 4.0
 * \param shadertype GLenum
 * \param location GLint
 * \param params GLuint *
 * \return void
 */
STATICINLINE void get_uniform_subroutineuiv(
    group::shader_type shadertype,
    i32                location,
    u32&               params,
    error_check        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetUniformSubroutineuiv)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glGetUniformSubroutineuiv, shadertype, location, params);
#endif
    glGetUniformSubroutineuiv(
        static_cast<GLenum>(shadertype), location, &params);
    detail::error_check("GetUniformSubroutineuiv"sv, check_errors);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<4, 0>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glUniformSubroutinesuiv. Introduced in GL core 4.0
 * \param shadertype GLenum
 * \param count GLsizei
 * \param indices const GLuint *
 * \return void
 */
STATICINLINE void uniform_subroutines(
    group::shader_type    shadertype,
    span_const_u32 const& indices,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(UniformSubroutinesuiv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformSubroutinesuiv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(indices.data()),
            indices.size_bytes()),
        shadertype);
#endif
    glUniformSubroutinesuiv(
        static_cast<GLenum>(shadertype),
        indices.size(),
        indices.size() ? reinterpret_cast<const GLuint*>(indices.data())
                       : nullptr);
    detail::error_check("UniformSubroutinesuiv"sv, check_errors);
}

template<class span_const_f32>
requires(
    MinimumVersion<Current, Version<4, 0>> && concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Wraps around glPatchParameterfv. Introduced in GL core 4.0
 * \param pname GLenum
 * \param values const GLfloat *
 * \return void
 */
STATICINLINE void patch_parameter(
    group::patch_parameter_name pname,
    span_const_f32 const&       values,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(PatchParameterfv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glPatchParameterfv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(values.data()), values.size_bytes()),
        pname);
#endif
    glPatchParameterfv(
        static_cast<GLenum>(pname),
        values.size() ? reinterpret_cast<const GLfloat*>(values.data())
                      : nullptr);
    detail::error_check("PatchParameterfv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>)
/*!
 * \brief Wraps around glPatchParameteri. Introduced in GL core 4.0
 * \param pname GLenum
 * \param value GLint
 * \return void
 */
STATICINLINE void patch_parameter(
    group::patch_parameter_name pname,
    i32                         value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(PatchParameteri)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glPatchParameteri, pname, value);
#endif
    glPatchParameteri(static_cast<GLenum>(pname), value);
    detail::error_check("PatchParameteri"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>)
/*!
 * \brief Wraps around glBindTransformFeedback. Introduced in GL core 4.0
 * \param target GLenum
 * \param id GLuint
 * \return void
 */
STATICINLINE void bind_transform_feedback(
    group::bind_transform_feedback_target target,
    u32                                   id,
    error_check                           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(BindTransformFeedback)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBindTransformFeedback, target, id);
#endif
    glBindTransformFeedback(static_cast<GLenum>(target), id);
    detail::error_check("BindTransformFeedback"sv, check_errors);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<4, 0>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glDeleteTransformFeedbacks. Introduced in GL core 4.0
 * \param n GLsizei
 * \param ids const GLuint *
 * \return void
 */
STATICINLINE void delete_transform_feedbacks(
    span_const_u32 const& ids, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(DeleteTransformFeedbacks)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glDeleteTransformFeedbacks,
        gsl::span<const char>(
            reinterpret_cast<const char*>(ids.data()), ids.size_bytes()));
#endif
    glDeleteTransformFeedbacks(
        ids.size(),
        ids.size() ? reinterpret_cast<const GLuint*>(ids.data()) : nullptr);
    detail::error_check("DeleteTransformFeedbacks"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>)
/*!
 * \brief Wraps around glDrawTransformFeedback. Introduced in GL core 4.0
 * \param mode GLenum
 * \param id GLuint
 * \return void
 */
STATICINLINE void draw_transform_feedback(
    group::primitive_type mode,
    u32                   id,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(DrawTransformFeedback)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glDrawTransformFeedback, mode, id);
#endif
    glDrawTransformFeedback(static_cast<GLenum>(mode), id);
    detail::error_check("DrawTransformFeedback"sv, check_errors);
}

template<class span_u32>
requires(
    MinimumVersion<Current, Version<4, 0>> && concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glGenTransformFeedbacks. Introduced in GL core 4.0
 * \param n GLsizei
 * \param ids GLuint *
 * \return void
 */
STATICINLINE void gen_transform_feedbacks(
    span_u32 ids, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GenTransformFeedbacks)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGenTransformFeedbacks,
        gsl::span<char>(reinterpret_cast<char*>(ids.data()), ids.size_bytes()));
#endif
    glGenTransformFeedbacks(
        ids.size(),
        ids.size() ? reinterpret_cast<GLuint*>(ids.data()) : nullptr);
    detail::error_check("GenTransformFeedbacks"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>)
/*!
 * \brief Wraps around glIsTransformFeedback. Introduced in GL core 4.0
 * \param id GLuint
 * \return GLboolean
 */
STATICINLINE GLboolean
is_transform_feedback(u32 id, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(IsTransformFeedback)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glIsTransformFeedback, id);
#endif
    auto out = glIsTransformFeedback(id);
    detail::error_check("IsTransformFeedback"sv, check_errors);
    return out;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>)
/*!
 * \brief Wraps around glPauseTransformFeedback. Introduced in GL core 4.0

 * \return void
 */
STATICINLINE void pause_transform_feedback(
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(PauseTransformFeedback)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glPauseTransformFeedback);
#endif
    glPauseTransformFeedback();
    detail::error_check("PauseTransformFeedback"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>)
/*!
 * \brief Wraps around glResumeTransformFeedback. Introduced in GL core 4.0

 * \return void
 */
STATICINLINE void resume_transform_feedback(
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ResumeTransformFeedback)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glResumeTransformFeedback);
#endif
    glResumeTransformFeedback();
    detail::error_check("ResumeTransformFeedback"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>)
/*!
 * \brief Wraps around glBeginQueryIndexed. Introduced in GL core 4.0
 * \param target GLenum
 * \param index GLuint
 * \param id GLuint
 * \return void
 */
STATICINLINE void begin_query_indexed(
    group::query_target target,
    u32                 index,
    u32                 id,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(BeginQueryIndexed)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBeginQueryIndexed, target, index, id);
#endif
    glBeginQueryIndexed(static_cast<GLenum>(target), index, id);
    detail::error_check("BeginQueryIndexed"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>)
/*!
 * \brief Wraps around glDrawTransformFeedbackStream. Introduced in GL core 4.0
 * \param mode GLenum
 * \param id GLuint
 * \param stream GLuint
 * \return void
 */
STATICINLINE void draw_transform_feedback_stream(
    group::primitive_type mode,
    u32                   id,
    u32                   stream,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(DrawTransformFeedbackStream)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glDrawTransformFeedbackStream, mode, id, stream);
#endif
    glDrawTransformFeedbackStream(static_cast<GLenum>(mode), id, stream);
    detail::error_check("DrawTransformFeedbackStream"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>)
/*!
 * \brief Wraps around glEndQueryIndexed. Introduced in GL core 4.0
 * \param target GLenum
 * \param index GLuint
 * \return void
 */
STATICINLINE void end_query_indexed(
    group::query_target target,
    u32                 index,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(EndQueryIndexed)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glEndQueryIndexed, target, index);
#endif
    glEndQueryIndexed(static_cast<GLenum>(target), index);
    detail::error_check("EndQueryIndexed"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<4, 0>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetQueryIndexediv. Introduced in GL core 4.0
 * \param target GLenum
 * \param index GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_query_indexediv(
    group::query_target         target,
    u32                         index,
    group::query_parameter_name pname,
    span_i32                    params,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetQueryIndexediv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetQueryIndexediv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        target,
        index,
        pname);
#endif
    glGetQueryIndexediv(
        static_cast<GLenum>(target),
        index,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetQueryIndexediv"sv, check_errors);
}

#endif // GL_VERSION_4_0
