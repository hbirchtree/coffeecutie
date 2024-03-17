#ifdef GL_VERSION_4_6
template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<4, 6>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glSpecializeShader. Introduced in GL core 4.6
 * \param shader GLuint
 * \param pEntryPoint const GLchar *
 * \param numSpecializationConstants GLuint
 * \param pConstantIndex const GLuint *
 * \param pConstantValue const GLuint *
 * \return void
 */
STATICINLINE void specialize_shader(
    u32                     shader,
    std::string_view const& pEntryPoint,
    span_const_u32 const&   pConstantIndex,
    span_const_u32 const&   pConstantValue,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SpecializeShader)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsShader)
            glIsShader(shader);
#endif
    }
    glSpecializeShader(
        shader,
        pEntryPoint.data(),
        pConstantIndex.size(),
        pConstantIndex.size()
            ? reinterpret_cast<const GLuint*>(pConstantIndex.data())
            : nullptr,
        pConstantValue.size()
            ? reinterpret_cast<const GLuint*>(pConstantValue.data())
            : nullptr);
    detail::error_check("SpecializeShader"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 6>>)
/*!
 * \brief Wraps around glMultiDrawArraysIndirectCount. Introduced in GL core 4.6
 * \param mode GLenum
 * \param indirect const void *
 * \param drawcount GLintptr
 * \param maxdrawcount GLsizei
 * \param stride GLsizei
 * \return void
 */
STATICINLINE void multi_draw_arrays_indirect_count(
    group::primitive_type mode,
    intptr_t              indirect,
    GLintptr              drawcount,
    i32                   maxdrawcount,
    i32                   stride,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawArraysIndirectCount)
    }
    glMultiDrawArraysIndirectCount(
        static_cast<GLenum>(mode),
        reinterpret_cast<const void*>(indirect),
        drawcount,
        maxdrawcount,
        stride);
    detail::error_check("MultiDrawArraysIndirectCount"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 6>>)
/*!
 * \brief Wraps around glMultiDrawElementsIndirectCount. Introduced in GL
 * core 4.6 \param mode GLenum \param type GLenum \param indirect const void *
 * \param drawcount GLintptr
 * \param maxdrawcount GLsizei
 * \param stride GLsizei
 * \return void
 */
STATICINLINE void multi_draw_elements_indirect_count(
    group::primitive_type     mode,
    group::draw_elements_type type,
    intptr_t                  indirect,
    GLintptr                  drawcount,
    i32                       maxdrawcount,
    i32                       stride,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawElementsIndirectCount)
    }
    glMultiDrawElementsIndirectCount(
        static_cast<GLenum>(mode),
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indirect),
        drawcount,
        maxdrawcount,
        stride);
    detail::error_check("MultiDrawElementsIndirectCount"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 6>>)
/*!
 * \brief Wraps around glPolygonOffsetClamp. Introduced in GL core 4.6
 * \param factor GLfloat
 * \param units GLfloat
 * \param clamp GLfloat
 * \return void
 */
STATICINLINE void polygon_offset_clamp(
    f32         factor,
    f32         units,
    f32         clamp,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PolygonOffsetClamp)
    }
    glPolygonOffsetClamp(factor, units, clamp);
    detail::error_check("PolygonOffsetClamp"sv, check_errors);
}

#endif // GL_VERSION_4_6
