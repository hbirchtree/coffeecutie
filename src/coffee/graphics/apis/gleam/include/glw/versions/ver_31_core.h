#ifdef GL_VERSION_3_1
/* Introduced in GL core 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
    void draw_arrays_instanced(
        group::primitive_type mode, i32 first, i32 count, i32 instancecount)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawArraysInstanced)
    }
    glDrawArraysInstanced(
        static_cast<GLenum>(mode), first, count, instancecount);
    detail::error_check("DrawArraysInstanced"sv);
}

/* Introduced in GL core 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
    void draw_elements_instanced(
        group::primitive_type     mode,
        i32                       count,
        group::draw_elements_type type,
        ptroff                    indices,
        i32                       instancecount)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawElementsInstanced)
    }
    glDrawElementsInstanced(
        static_cast<GLenum>(mode),
        count,
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices),
        instancecount);
    detail::error_check("DrawElementsInstanced"sv);
}

/* Introduced in GL core 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
    void primitive_restart_index(u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PrimitiveRestartIndex)
    }
    glPrimitiveRestartIndex(index);
    detail::error_check("PrimitiveRestartIndex"sv);
}

/* Introduced in GL core 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE void tex_buffer(
    group::texture_target        target,
    group::sized_internal_format internalformat,
    u32                          buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexBuffer)
        glIsBuffer(buffer);
    }
    glTexBuffer(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        buffer);
    detail::error_check("TexBuffer"sv);
}

/* Introduced in GL core 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
    void copy_buffer_sub_data(
        group::copy_buffer_sub_data_target readTarget,
        group::copy_buffer_sub_data_target writeTarget,
        GLintptr                           readOffset,
        GLintptr                           writeOffset,
        GLsizeiptr                         size)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyBufferSubData)
    }
    glCopyBufferSubData(
        static_cast<GLenum>(readTarget),
        static_cast<GLenum>(writeTarget),
        readOffset,
        writeOffset,
        size);
    detail::error_check("CopyBufferSubData"sv);
}

/* Introduced in GL core 3.1 */
template<class span_GLchar>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>) STATICINLINE
    void get_active_uniform_block_name(
        u32         program,
        u32         uniformBlockIndex,
        i32&        length,
        span_GLchar uniformBlockName)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveUniformBlockName)
        glIsProgram(program);
    }
    glGetActiveUniformBlockName(
        program,
        uniformBlockIndex,
        uniformBlockName.size(),
        &length,
        uniformBlockName.data());
    detail::error_check("GetActiveUniformBlockName"sv);
}

/* Introduced in GL core 3.1 */
template<class span_i32>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_active_uniform_blockiv(
        u32                       program,
        u32                       uniformBlockIndex,
        group::uniform_block_prop pname,
        span_i32                  params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveUniformBlockiv)
        glIsProgram(program);
    }
    glGetActiveUniformBlockiv(
        program,
        uniformBlockIndex,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetActiveUniformBlockiv"sv);
}

/* Introduced in GL core 3.1 */
template<class span_GLchar>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>) STATICINLINE
    void get_active_uniform_name(
        u32 program, u32 uniformIndex, i32& length, span_GLchar uniformName)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveUniformName)
        glIsProgram(program);
    }
    glGetActiveUniformName(
        program, uniformIndex, uniformName.size(), &length, uniformName.data());
    detail::error_check("GetActiveUniformName"sv);
}

/* Introduced in GL core 3.1 */
template<class span_const_u32, class span_i32>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>&& semantic::concepts::Span<span_i32>&&
                                      std::is_same_v<
                     std::decay_t<typename span_i32::value_type>,
                     std::decay_t<i32>>) STATICINLINE
    void get_active_uniformsiv(
        u32                   program,
        span_const_u32 const& uniformIndices,
        group::uniform_prop   pname,
        span_i32              params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveUniformsiv)
        glIsProgram(program);
    }
    glGetActiveUniformsiv(
        program,
        uniformIndices.size(),
        uniformIndices.size()
            ? reinterpret_cast<const GLuint*>(uniformIndices.data())
            : nullptr,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetActiveUniformsiv"sv);
}

/* Introduced in GL core 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE GLuint
    get_uniform_block_index(
        u32 program, std::string_view const& uniformBlockName)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformBlockIndex)
        glIsProgram(program);
    }
    auto out = glGetUniformBlockIndex(program, uniformBlockName.data());
    detail::error_check("GetUniformBlockIndex"sv);
    return out;
}

/* Introduced in GL core 3.1 */
template<class span_u32>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void get_uniform_indices(
        u32                           program,
        i32                           uniformCount,
        std::vector<std::string_view> uniformNames,
        span_u32                      uniformIndices)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformIndices)
        glIsProgram(program);
    }
    auto [uniformNames_lens, uniformNames_cstr, uniformNames_store] =
        detail::transform_strings(uniformNames);
    glGetUniformIndices(
        program,
        uniformCount,
        uniformNames_cstr.data(),
        uniformIndices.size() ? reinterpret_cast<GLuint*>(uniformIndices.data())
                              : nullptr);
    detail::error_check("GetUniformIndices"sv);
}

/* Introduced in GL core 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
    void uniform_block_binding(
        u32 program, u32 uniformBlockIndex, u32 uniformBlockBinding)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformBlockBinding)
        glIsProgram(program);
    }
    glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);
    detail::error_check("UniformBlockBinding"sv);
}

#endif // GL_VERSION_3_1