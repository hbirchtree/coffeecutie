#ifdef GL_VERSION_1_1
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 1>>)
/*!
 * \brief Wraps around glBindTexture. Introduced in GL core 1.1
 * \param target GLenum
 * \param texture GLuint
 * \return void
 */
STATICINLINE void bind_texture(group::texture_target target, u32 texture)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindTexture)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glBindTexture(static_cast<GLenum>(target), texture);
    detail::error_check("BindTexture"sv);
}

template<class vec_2_i32>
requires(
    MinimumVersion<Current, Version<1, 1>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
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
    i32                    border)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTexImage1D)
    }
    glCopyTexImage1D(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        x.x(),
        x.y(),
        width,
        border);
    detail::error_check("CopyTexImage1D"sv);
}

template<class size_2_i32, class vec_2_i32>
requires(
    MinimumVersion<Current, Version<1, 1>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2> &&
    semantic::concepts::Size2D<size_2_i32, i32>)
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
    i32                    border)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTexImage2D)
    }
    glCopyTexImage2D(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        x.x(),
        x.y(),
        width[0],
        width[1],
        border);
    detail::error_check("CopyTexImage2D"sv);
}

template<class vec_2_i32>
requires(
    MinimumVersion<Current, Version<1, 1>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
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
    i32                   width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTexSubImage1D)
    }
    glCopyTexSubImage1D(
        static_cast<GLenum>(target), level, xoffset, x.x(), x.y(), width);
    detail::error_check("CopyTexSubImage1D"sv);
}

template<class size_2_i32, class vec_2_i32>
requires(
    MinimumVersion<Current, Version<1, 1>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2> &&
    semantic::concepts::Size2D<size_2_i32, i32>)
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
    size_2_i32 const&     width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTexSubImage2D)
    }
    glCopyTexSubImage2D(
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        x.x(),
        x.y(),
        width[0],
        width[1]);
    detail::error_check("CopyTexSubImage2D"sv);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<1, 1>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glDeleteTextures. Introduced in GL core 1.1
 * \param n GLsizei
 * \param textures const GLuint *
 * \return void
 */
STATICINLINE void delete_textures(span_const_u32 const& textures)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteTextures)
    }
    glDeleteTextures(
        textures.size(),
        textures.size() ? reinterpret_cast<const GLuint*>(textures.data())
                        : nullptr);
    detail::error_check("DeleteTextures"sv);
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
STATICINLINE void draw_arrays(group::primitive_type mode, i32 first, i32 count)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawArrays)
    }
    glDrawArrays(static_cast<GLenum>(mode), first, count);
    detail::error_check("DrawArrays"sv);
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
    ptroff                    indices)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawElements)
    }
    glDrawElements(
        static_cast<GLenum>(mode),
        count,
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices));
    detail::error_check("DrawElements"sv);
}

template<class span_u32>
requires(
    MinimumVersion<Current, Version<1, 1>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glGenTextures. Introduced in GL core 1.1
 * \param n GLsizei
 * \param textures GLuint *
 * \return void
 */
STATICINLINE void gen_textures(span_u32 textures)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenTextures)
    }
    glGenTextures(
        textures.size(),
        textures.size() ? reinterpret_cast<GLuint*>(textures.data()) : nullptr);
    detail::error_check("GenTextures"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 1>>)
/*!
 * \brief Wraps around glIsTexture. Introduced in GL core 1.1
 * \param texture GLuint
 * \return Boolean
 */
STATICINLINE GLboolean is_texture(u32 texture)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsTexture)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    auto out = glIsTexture(texture);
    detail::error_check("IsTexture"sv);
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
STATICINLINE void polygon_offset(f32 factor, f32 units)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PolygonOffset)
    }
    glPolygonOffset(factor, units);
    detail::error_check("PolygonOffset"sv);
}

template<class span_const_void>
requires(
    MinimumVersion<Current, Version<1, 1>> &&
    semantic::concepts::Span<span_const_void>)
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
    span_const_void const& pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexSubImage1D)
    }
    glTexSubImage1D(
        static_cast<GLenum>(target),
        level,
        xoffset,
        width,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TexSubImage1D"sv);
}

template<class size_2_i32, class span_const_void, class vec_2_i32>
requires(
    MinimumVersion<Current, Version<1, 1>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2> &&
    semantic::concepts::Size2D<size_2_i32, i32> &&
    semantic::concepts::Span<span_const_void>)
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
    span_const_void const& pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexSubImage2D)
    }
    glTexSubImage2D(
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TexSubImage2D"sv);
}

#endif // GL_VERSION_1_1
