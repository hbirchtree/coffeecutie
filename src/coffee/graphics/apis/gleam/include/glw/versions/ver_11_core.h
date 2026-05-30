#ifdef GL_VERSION_1_1
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 1>>)
/*!
 * \brief Wraps around glBindTexture. Introduced in GL core 1.1
 * \param target GLenum
 * \param texture GLuint
 * \return void
 */
STATICINLINE void bind_texture(
    group::texture_target target,
    u32                   texture,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindTexture)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBindTexture, target, texture);
#endif
    glBindTexture(static_cast<GLenum>(target), texture);
    detail::error_check("BindTexture"sv, check_errors);
}

template<class vec_2_i32>
requires(
    MinimumVersion<Current, Version<1, 1>> &&
    concepts::vector<vec_2_i32, i32, 2>)
/*!
 * \brief Wraps around glCopyTexImage1D. Introduced in GL core 1.1
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLenum
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \param border GLint
 * \return void
 */
STATICINLINE void copy_tex_image_1d(
    group::texture_target  target,
    i32                    level,
    group::internal_format internalformat,
    vec_2_i32 const&       x,
    i32                    width,
    i32                    border,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTexImage1D)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glCopyTexImage1D, target, level, internalformat, x, width, border);
#endif
    glCopyTexImage1D(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        x[0],
        x[1],
        width,
        border);
    detail::error_check("CopyTexImage1D"sv, check_errors);
}

template<class size_2_i32, class vec_2_i32>
requires(
    MinimumVersion<Current, Version<1, 1>> &&
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Wraps around glCopyTexImage2D. Introduced in GL core 1.1
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLenum
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param border GLint
 * \return void
 */
STATICINLINE void copy_tex_image_2d(
    group::texture_target  target,
    i32                    level,
    group::internal_format internalformat,
    vec_2_i32 const&       x,
    size_2_i32 const&      width,
    i32                    border,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTexImage2D)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glCopyTexImage2D, target, level, internalformat, x, width, border);
#endif
    glCopyTexImage2D(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        x[0],
        x[1],
        width[0],
        width[1],
        border);
    detail::error_check("CopyTexImage2D"sv, check_errors);
}

template<class vec_2_i32>
requires(
    MinimumVersion<Current, Version<1, 1>> &&
    concepts::vector<vec_2_i32, i32, 2>)
/*!
 * \brief Wraps around glCopyTexSubImage1D. Introduced in GL core 1.1
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \return void
 */
STATICINLINE void copy_tex_sub_image_1d(
    group::texture_target target,
    i32                   level,
    i32                   xoffset,
    vec_2_i32 const&      x,
    i32                   width,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTexSubImage1D)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glCopyTexSubImage1D, target, level, xoffset, x, width);
#endif
    glCopyTexSubImage1D(
        static_cast<GLenum>(target), level, xoffset, x[0], x[1], width);
    detail::error_check("CopyTexSubImage1D"sv, check_errors);
}

template<class size_2_i32, class vec_2_i32>
requires(
    MinimumVersion<Current, Version<1, 1>> &&
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Wraps around glCopyTexSubImage2D. Introduced in GL core 1.1
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param yoffset GLint
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \param height GLsizei
 * \return void
 */
STATICINLINE void copy_tex_sub_image_2d(
    group::texture_target target,
    i32                   level,
    vec_2_i32 const&      xoffset,
    vec_2_i32 const&      x,
    size_2_i32 const&     width,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTexSubImage2D)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glCopyTexSubImage2D, target, level, xoffset, x, width);
#endif
    glCopyTexSubImage2D(
        static_cast<GLenum>(target),
        level,
        xoffset[0],
        xoffset[1],
        x[0],
        x[1],
        width[0],
        width[1]);
    detail::error_check("CopyTexSubImage2D"sv, check_errors);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<1, 1>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glDeleteTextures. Introduced in GL core 1.1
 * \param n GLsizei
 * \param textures const GLuint *
 * \return void
 */
STATICINLINE void delete_textures(
    span_const_u32 const& textures, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteTextures)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glDeleteTextures,
        gsl::span<const char>(
            reinterpret_cast<const char*>(textures.data()),
            textures.size_bytes()));
#endif
    glDeleteTextures(
        textures.size(),
        textures.size() ? reinterpret_cast<const GLuint*>(textures.data())
                        : nullptr);
    detail::error_check("DeleteTextures"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 1>>)
/*!
 * \brief Wraps around glDrawArrays. Introduced in GL core 1.1
 * \param mode GLenum
 * \param first GLint
 * \param count GLsizei
 * \return void
 */
STATICINLINE void draw_arrays(
    group::primitive_type mode,
    i32                   first,
    i32                   count,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawArrays)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glDrawArrays, mode, first, count);
#endif
    glDrawArrays(static_cast<GLenum>(mode), first, count);
    detail::error_check("DrawArrays"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 1>>)
/*!
 * \brief Wraps around glDrawElements. Introduced in GL core 1.1
 * \param mode GLenum
 * \param count GLsizei
 * \param type GLenum
 * \param indices const void *
 * \return void
 */
STATICINLINE void draw_elements(
    group::primitive_type     mode,
    i32                       count,
    group::draw_elements_type type,
    intptr_t                  indices,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawElements)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glDrawElements, mode, count, type, indices);
#endif
    glDrawElements(
        static_cast<GLenum>(mode),
        count,
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices));
    detail::error_check("DrawElements"sv, check_errors);
}

template<class span_u32>
requires(
    MinimumVersion<Current, Version<1, 1>> && concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glGenTextures. Introduced in GL core 1.1
 * \param n GLsizei
 * \param textures GLuint *
 * \return void
 */
STATICINLINE void gen_textures(
    span_u32 textures, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenTextures)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGenTextures,
        gsl::span<char>(
            reinterpret_cast<char*>(textures.data()), textures.size_bytes()));
#endif
    glGenTextures(
        textures.size(),
        textures.size() ? reinterpret_cast<GLuint*>(textures.data()) : nullptr);
    detail::error_check("GenTextures"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 1>>)
/*!
 * \brief Wraps around glIsTexture. Introduced in GL core 1.1
 * \param texture GLuint
 * \return GLboolean
 */
STATICINLINE GLboolean
is_texture(u32 texture, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsTexture)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glIsTexture, texture);
#endif
    auto out = glIsTexture(texture);
    detail::error_check("IsTexture"sv, check_errors);
    return out;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 1>>)
/*!
 * \brief Wraps around glPolygonOffset. Introduced in GL core 1.1
 * \param factor GLfloat
 * \param units GLfloat
 * \return void
 */
STATICINLINE void polygon_offset(
    f32 factor, f32 units, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PolygonOffset)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glPolygonOffset, factor, units);
#endif
    glPolygonOffset(factor, units);
    detail::error_check("PolygonOffset"sv, check_errors);
}

template<class span_const_void>
requires(
    MinimumVersion<Current, Version<1, 1>> && concepts::span<span_const_void>)
/*!
 * \brief Wraps around glTexSubImage1D. Introduced in GL core 1.1
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param width GLsizei
 * \param format GLenum
 * \param type GLenum
 * \param pixels const void *
 * \return void
 */
STATICINLINE void tex_sub_image_1d(
    group::texture_target  target,
    i32                    level,
    i32                    xoffset,
    i32                    width,
    group::pixel_format    format,
    group::pixel_type      type,
    span_const_void const& pixels,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexSubImage1D)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glTexSubImage1D,
        gsl::span<const char>(
            reinterpret_cast<const char*>(pixels.data()), pixels.size_bytes()),
        target,
        level,
        xoffset,
        width,
        format,
        type);
#endif
    glTexSubImage1D(
        static_cast<GLenum>(target),
        level,
        xoffset,
        width,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TexSubImage1D"sv, check_errors);
}

template<class size_2_i32, class span_const_void, class vec_2_i32>
requires(
    MinimumVersion<Current, Version<1, 1>> &&
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32> &&
    concepts::span<span_const_void>)
/*!
 * \brief Wraps around glTexSubImage2D. Introduced in GL core 1.1
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param yoffset GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param format GLenum
 * \param type GLenum
 * \param pixels const void *
 * \return void
 */
STATICINLINE void tex_sub_image_2d(
    group::texture_target  target,
    i32                    level,
    vec_2_i32 const&       xoffset,
    size_2_i32 const&      width,
    group::pixel_format    format,
    group::pixel_type      type,
    span_const_void const& pixels,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexSubImage2D)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glTexSubImage2D,
        gsl::span<const char>(
            reinterpret_cast<const char*>(pixels.data()), pixels.size_bytes()),
        target,
        level,
        xoffset,
        width,
        format,
        type);
#endif
    glTexSubImage2D(
        static_cast<GLenum>(target),
        level,
        xoffset[0],
        xoffset[1],
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TexSubImage2D"sv, check_errors);
}

#endif // GL_VERSION_1_1
