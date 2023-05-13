#ifdef GL_VERSION_3_1
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glDrawArraysInstanced. Introduced in GL core 3.1
     * \param mode GLenum
     * \param first GLint
     * \param count GLsizei
     * \param instancecount GLsizei
     * \return void
     */
    STATICINLINE void draw_arrays_instanced(
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

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glDrawElementsInstanced. Introduced in GL core 3.1
     * \param mode GLenum
     * \param count GLsizei
     * \param type GLenum
     * \param indices const void *
     * \param instancecount GLsizei
     * \return void
     */
    STATICINLINE void draw_elements_instanced(
        group::primitive_type     mode,
        i32                       count,
        group::draw_elements_type type,
        intptr_t                  indices,
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

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glPrimitiveRestartIndex. Introduced in GL core 3.1
     * \param index GLuint
     * \return void
     */
    STATICINLINE void primitive_restart_index(u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PrimitiveRestartIndex)
    }
    glPrimitiveRestartIndex(index);
    detail::error_check("PrimitiveRestartIndex"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glTexBuffer. Introduced in GL core 3.1
     * \param target GLenum
     * \param internalformat GLenum
     * \param buffer GLuint
     * \return void
     */
    STATICINLINE void tex_buffer(
        group::texture_target        target,
        group::sized_internal_format internalformat,
        u32                          buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexBuffer)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glTexBuffer(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        buffer);
    detail::error_check("TexBuffer"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glCopyBufferSubData. Introduced in GL core 3.1
     * \param readTarget GLenum
     * \param writeTarget GLenum
     * \param readOffset GLintptr
     * \param writeOffset GLintptr
     * \param size GLsizeiptr
     * \return void
     */
    STATICINLINE void copy_buffer_sub_data(
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

template<class span_GLchar>
requires(MinimumVersion<Current, Version<3, 1>>&& concepts::span<span_GLchar>&&
                                                  std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>)
    /*!
     * \brief Wraps around glGetActiveUniformBlockName. Introduced in GL
     * core 3.1 \param program GLuint \param uniformBlockIndex GLuint \param
     * bufSize GLsizei \param length GLsizei * \param uniformBlockName GLchar *
     * \return void
     */
    STATICINLINE void get_active_uniform_block_name(
        u32         program,
        u32         uniformBlockIndex,
        i32&        length,
        span_GLchar uniformBlockName)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveUniformBlockName)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetActiveUniformBlockName(
        program,
        uniformBlockIndex,
        uniformBlockName.size(),
        &length,
        uniformBlockName.data());
    detail::error_check("GetActiveUniformBlockName"sv);
}

template<class span_i32>
requires(MinimumVersion<Current, Version<3, 1>>&& concepts::span<span_i32>&&
                                                  std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Wraps around glGetActiveUniformBlockiv. Introduced in GL core 3.1
     * \param program GLuint
     * \param uniformBlockIndex GLuint
     * \param pname GLenum
     * \param params GLint *
     * \return void
     */
    STATICINLINE void get_active_uniform_blockiv(
        u32                       program,
        u32                       uniformBlockIndex,
        group::uniform_block_prop pname,
        span_i32                  params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveUniformBlockiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetActiveUniformBlockiv(
        program,
        uniformBlockIndex,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetActiveUniformBlockiv"sv);
}

template<class span_GLchar>
requires(MinimumVersion<Current, Version<3, 1>>&& concepts::span<span_GLchar>&&
                                                  std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>)
    /*!
     * \brief Wraps around glGetActiveUniformName. Introduced in GL core 3.1
     * \param program GLuint
     * \param uniformIndex GLuint
     * \param bufSize GLsizei
     * \param length GLsizei *
     * \param uniformName GLchar *
     * \return void
     */
    STATICINLINE void get_active_uniform_name(
        u32 program, u32 uniformIndex, i32& length, span_GLchar uniformName)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveUniformName)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetActiveUniformName(
        program, uniformIndex, uniformName.size(), &length, uniformName.data());
    detail::error_check("GetActiveUniformName"sv);
}

template<class span_const_u32, class span_i32>
requires(MinimumVersion<Current, Version<3, 1>>&&
             concepts::span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>&& concepts::span<span_i32>&&
                                              std::is_same_v<
                     std::decay_t<typename span_i32::value_type>,
                     std::decay_t<i32>>)
    /*!
     * \brief Wraps around glGetActiveUniformsiv. Introduced in GL core 3.1
     * \param program GLuint
     * \param uniformCount GLsizei
     * \param uniformIndices const GLuint *
     * \param pname GLenum
     * \param params GLint *
     * \return void
     */
    STATICINLINE void get_active_uniformsiv(
        u32                   program,
        span_const_u32 const& uniformIndices,
        group::uniform_prop   pname,
        span_i32              params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveUniformsiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
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

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glGetUniformBlockIndex. Introduced in GL core 3.1
     * \param program GLuint
     * \param uniformBlockName const GLchar *
     * \return GLuint
     */
    STATICINLINE GLuint get_uniform_block_index(
        u32 program, std::string_view const& uniformBlockName)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformBlockIndex)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    auto out = glGetUniformBlockIndex(program, uniformBlockName.data());
    detail::error_check("GetUniformBlockIndex"sv);
    return out;
}

template<class span_u32>
requires(MinimumVersion<Current, Version<3, 1>>&& concepts::span<span_u32>&&
                                                  std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>)
    /*!
     * \brief Wraps around glGetUniformIndices. Introduced in GL core 3.1
     * \param program GLuint
     * \param uniformCount GLsizei
     * \param uniformNames const GLchar *const*
     * \param uniformIndices GLuint *
     * \return void
     */
    STATICINLINE void get_uniform_indices(
        u32                           program,
        i32                           uniformCount,
        std::vector<std::string_view> uniformNames,
        span_u32                      uniformIndices)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformIndices)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
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

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glUniformBlockBinding. Introduced in GL core 3.1
     * \param program GLuint
     * \param uniformBlockIndex GLuint
     * \param uniformBlockBinding GLuint
     * \return void
     */
    STATICINLINE void uniform_block_binding(
        u32 program, u32 uniformBlockIndex, u32 uniformBlockBinding)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformBlockBinding)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);
    detail::error_check("UniformBlockBinding"sv);
}

#endif // GL_VERSION_3_1
