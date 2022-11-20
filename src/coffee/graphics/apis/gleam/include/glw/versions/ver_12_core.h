#ifdef GL_VERSION_1_2
template<class size_2_i32, class vec_2_i32, class vec_3_i32>
requires(MinimumVersion<Current, Version<1, 2>>&&
                     semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                     semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                     semantic::concepts::Size2D<size_2_i32, i32>)
    /*!
     * \brief Wraps around glCopyTexSubImage3D. Introduced in GL core 1.2
     * \param target GLenum
     * \param level GLint
     * \param xoffset GLint
     * \param yoffset GLint
     * \param zoffset GLint
     * \param x GLint
     * \param y GLint
     * \param width GLsizei
     * \param height GLsizei
     * \return void
     */
    STATICINLINE void copy_tex_sub_image_3d(
        group::texture_target target,
        i32                   level,
        vec_3_i32 const&      xoffset,
        vec_2_i32 const&      x,
        size_2_i32 const&     width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTexSubImage3D)
    }
    glCopyTexSubImage3D(
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        x.x(),
        x.y(),
        width[0],
        width[1]);
    detail::error_check("CopyTexSubImage3D"sv);
}

template<class span_const_void>
requires(MinimumVersion<Current, Version<1, 2>>&&
             semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Wraps around glDrawRangeElements. Introduced in GL core 1.2
     * \param mode GLenum
     * \param start GLuint
     * \param end GLuint
     * \param count GLsizei
     * \param type GLenum
     * \param indices const void *
     * \return void
     */
    STATICINLINE void draw_range_elements(
        group::primitive_type     mode,
        u32                       start,
        u32                       end,
        i32                       count,
        group::draw_elements_type type,
        span_const_void const&    indices)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawRangeElements)
    }
    glDrawRangeElements(
        static_cast<GLenum>(mode),
        start,
        end,
        count,
        static_cast<GLenum>(type),
        indices.size() ? reinterpret_cast<const void*>(indices.data())
                       : nullptr);
    detail::error_check("DrawRangeElements"sv);
}

template<class size_3_i32, class span_const_void>
requires(MinimumVersion<Current, Version<1, 2>>&&
                 semantic::concepts::Size2D<size_3_i32, i32>&&
                 semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Wraps around glTexImage3D. Introduced in GL core 1.2
     * \param target GLenum
     * \param level GLint
     * \param internalformat GLint
     * \param width GLsizei
     * \param height GLsizei
     * \param depth GLsizei
     * \param border GLint
     * \param format GLenum
     * \param type GLenum
     * \param pixels const void *
     * \return void
     */
    STATICINLINE void tex_image_3d(
        group::texture_target  target,
        i32                    level,
        i32                    internalformat,
        size_3_i32 const&      width,
        i32                    border,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexImage3D)
    }
    glTexImage3D(
        static_cast<GLenum>(target),
        level,
        internalformat,
        width[0],
        width[1],
        width[2],
        border,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TexImage3D"sv);
}

template<class size_3_i32, class span_const_void, class vec_3_i32>
requires(MinimumVersion<Current, Version<1, 2>>&&
                     semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                     semantic::concepts::Size2D<size_3_i32, i32>&&
                     semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Wraps around glTexSubImage3D. Introduced in GL core 1.2
     * \param target GLenum
     * \param level GLint
     * \param xoffset GLint
     * \param yoffset GLint
     * \param zoffset GLint
     * \param width GLsizei
     * \param height GLsizei
     * \param depth GLsizei
     * \param format GLenum
     * \param type GLenum
     * \param pixels const void *
     * \return void
     */
    STATICINLINE void tex_sub_image_3d(
        group::texture_target  target,
        i32                    level,
        vec_3_i32 const&       xoffset,
        size_3_i32 const&      width,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexSubImage3D)
    }
    glTexSubImage3D(
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        width[0],
        width[1],
        width[2],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TexSubImage3D"sv);
}

#endif // GL_VERSION_1_2
