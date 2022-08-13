#ifdef GL_VERSION_1_3
/* Introduced in GL core 1.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 3>>) STATICINLINE
    void active_texture(group::texture_unit texture)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ActiveTexture)
    }
    glActiveTexture(static_cast<GLenum>(texture));
    detail::error_check("ActiveTexture"sv);
}

/* Introduced in GL core 1.3 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<1, 3>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_tex_image_1d(
        group::texture_target  target,
        i32                    level,
        group::internal_format internalformat,
        i32                    width,
        i32                    border,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTexImage1D)
    }
    glCompressedTexImage1D(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        width,
        border,
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTexImage1D"sv);
}

/* Introduced in GL core 1.3 */
template<class size_2_i32, class span_const_void>
requires(MinimumVersion<Current, Version<1, 3>>&&
                 semantic::concepts::Size2D<size_2_i32, i32>&&
                 semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_tex_image_2d(
        group::texture_target  target,
        i32                    level,
        group::internal_format internalformat,
        size_2_i32 const&      width,
        i32                    border,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTexImage2D)
    }
    glCompressedTexImage2D(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        border,
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTexImage2D"sv);
}

/* Introduced in GL core 1.3 */
template<class size_3_i32, class span_const_void>
requires(MinimumVersion<Current, Version<1, 3>>&&
                 semantic::concepts::Size2D<size_3_i32, i32>&&
                 semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_tex_image_3d(
        group::texture_target  target,
        i32                    level,
        group::internal_format internalformat,
        size_3_i32 const&      width,
        i32                    border,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTexImage3D)
    }
    glCompressedTexImage3D(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        width[2],
        border,
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTexImage3D"sv);
}

/* Introduced in GL core 1.3 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<1, 3>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_tex_sub_image_1d(
        group::texture_target  target,
        i32                    level,
        i32                    xoffset,
        i32                    width,
        group::pixel_format    format,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTexSubImage1D)
    }
    glCompressedTexSubImage1D(
        static_cast<GLenum>(target),
        level,
        xoffset,
        width,
        static_cast<GLenum>(format),
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTexSubImage1D"sv);
}

/* Introduced in GL core 1.3 */
template<class size_2_i32, class span_const_void, class vec_2_i32>
requires(MinimumVersion<Current, Version<1, 3>>&&
                     semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                     semantic::concepts::Size2D<size_2_i32, i32>&&
                     semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_tex_sub_image_2d(
        group::texture_target  target,
        i32                    level,
        vec_2_i32 const&       xoffset,
        size_2_i32 const&      width,
        group::pixel_format    format,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTexSubImage2D)
    }
    glCompressedTexSubImage2D(
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        width[0],
        width[1],
        static_cast<GLenum>(format),
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTexSubImage2D"sv);
}

/* Introduced in GL core 1.3 */
template<class size_3_i32, class span_const_void, class vec_3_i32>
requires(MinimumVersion<Current, Version<1, 3>>&&
                     semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                     semantic::concepts::Size2D<size_3_i32, i32>&&
                     semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_tex_sub_image_3d(
        group::texture_target  target,
        i32                    level,
        vec_3_i32 const&       xoffset,
        size_3_i32 const&      width,
        group::pixel_format    format,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTexSubImage3D)
    }
    glCompressedTexSubImage3D(
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        width[0],
        width[1],
        width[2],
        static_cast<GLenum>(format),
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTexSubImage3D"sv);
}

/* Introduced in GL core 1.3 */
template<class span_void>
requires(MinimumVersion<Current, Version<1, 3>>&&
             semantic::concepts::Span<span_void>) STATICINLINE
    void get_compressed_tex_image(
        group::texture_target target, i32 level, span_void img)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetCompressedTexImage)
    }
    glGetCompressedTexImage(
        static_cast<GLenum>(target),
        level,
        img.size() ? reinterpret_cast<void*>(img.data()) : nullptr);
    detail::error_check("GetCompressedTexImage"sv);
}

/* Introduced in GL core 1.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 3>>) STATICINLINE
    void sample_coverage(f32 value, bool invert)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SampleCoverage)
    }
    glSampleCoverage(value, invert);
    detail::error_check("SampleCoverage"sv);
}

#endif // GL_VERSION_1_3