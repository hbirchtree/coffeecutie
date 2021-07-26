#ifdef GL_VERSION_4_6
/* Introduced in GL core 4.6 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<4, 6>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void specialize_shader(
        u32                     shader,
        std::string_view const& pEntryPoint,
        u32                     numSpecializationConstants,
        span_const_u32 const&   pConstantIndex,
        span_const_u32 const&   pConstantValue)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SpecializeShader)
        glIsShader(shader);
    }
    glSpecializeShader(
        shader,
        pEntryPoint.data(),
        numSpecializationConstants,
        pConstantIndex.size()
            ? reinterpret_cast<const GLuint*>(pConstantIndex.data())
            : nullptr,
        pConstantValue.size()
            ? reinterpret_cast<const GLuint*>(pConstantValue.data())
            : nullptr);
    detail::error_check("SpecializeShader"sv);
}

/* Introduced in GL core 4.6 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 6>>) STATICINLINE
    void multi_draw_arrays_indirect_count(
        group::primitive_type mode,
        ptroff                indirect,
        GLintptr              drawcount,
        i32                   maxdrawcount,
        i32                   stride)
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
    detail::error_check("MultiDrawArraysIndirectCount"sv);
}

/* Introduced in GL core 4.6 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 6>>) STATICINLINE
    void multi_draw_elements_indirect_count(
        group::primitive_type     mode,
        group::draw_elements_type type,
        ptroff                    indirect,
        GLintptr                  drawcount,
        i32                       maxdrawcount,
        i32                       stride)
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
    detail::error_check("MultiDrawElementsIndirectCount"sv);
}

/* Introduced in GL core 4.6 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 6>>) STATICINLINE
    void polygon_offset_clamp(f32 factor, f32 units, f32 clamp)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PolygonOffsetClamp)
    }
    glPolygonOffsetClamp(factor, units, clamp);
    detail::error_check("PolygonOffsetClamp"sv);
}

#endif // GL_VERSION_4_6
