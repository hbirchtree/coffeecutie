#ifdef GL_VERSION_3_2
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glDrawElementsBaseVertex. Introduced in GL core 3.2
 * \param mode GLenum
 * \param count GLsizei
 * \param type GLenum
 * \param indices const void *
 * \param basevertex GLint
 * \return void
 */
STATICINLINE void draw_elements_base_vertex(
    group::primitive_type     mode,
    i32                       count,
    group::draw_elements_type type,
    ptroff                    indices,
    i32                       basevertex)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawElementsBaseVertex)
    }
    glDrawElementsBaseVertex(
        static_cast<GLenum>(mode),
        count,
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices),
        basevertex);
    detail::error_check("DrawElementsBaseVertex"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glDrawElementsInstancedBaseVertex. Introduced in GL
 * core 3.2 \param mode GLenum \param count GLsizei \param type GLenum \param
 * indices const void * \param instancecount GLsizei \param basevertex GLint
 * \return void
 */
STATICINLINE void draw_elements_instanced_base_vertex(
    group::primitive_type     mode,
    i32                       count,
    group::draw_elements_type type,
    ptroff                    indices,
    i32                       instancecount,
    i32                       basevertex)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawElementsInstancedBaseVertex)
    }
    glDrawElementsInstancedBaseVertex(
        static_cast<GLenum>(mode),
        count,
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices),
        instancecount,
        basevertex);
    detail::error_check("DrawElementsInstancedBaseVertex"sv);
}

template<class span_const_void>
requires(
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_const_void>)
/*!
 * \brief Wraps around glDrawRangeElementsBaseVertex. Introduced in GL core 3.2
 * \param mode GLenum
 * \param start GLuint
 * \param end GLuint
 * \param count GLsizei
 * \param type GLenum
 * \param indices const void *
 * \param basevertex GLint
 * \return void
 */
STATICINLINE void draw_range_elements_base_vertex(
    group::primitive_type     mode,
    u32                       start,
    u32                       end,
    i32                       count,
    group::draw_elements_type type,
    span_const_void const&    indices,
    i32                       basevertex)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawRangeElementsBaseVertex)
    }
    glDrawRangeElementsBaseVertex(
        static_cast<GLenum>(mode),
        start,
        end,
        count,
        static_cast<GLenum>(type),
        indices.size() ? reinterpret_cast<const void*>(indices.data())
                       : nullptr,
        basevertex);
    detail::error_check("DrawRangeElementsBaseVertex"sv);
}

template<class span_const_i32>
requires(
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glMultiDrawElementsBaseVertex. Introduced in GL core 3.2
 * \param mode GLenum
 * \param count const GLsizei *
 * \param type GLenum
 * \param indices const void *const*
 * \param drawcount GLsizei
 * \param basevertex const GLint *
 * \return void
 */
STATICINLINE void multi_draw_elements_base_vertex(
    group::primitive_type     mode,
    span_const_i32            count,
    group::draw_elements_type type,
    ptroff                    indices,
    span_const_i32            basevertex)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawElementsBaseVertex)
    }
    GLsizei drawcount = count.size();
    detail::assert_equal(count.size(), drawcount);
    detail::assert_equal(basevertex.size(), drawcount);
    glMultiDrawElementsBaseVertex(
        static_cast<GLenum>(mode),
        count.data(),
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices),
        basevertex.data());
    detail::error_check("MultiDrawElementsBaseVertex"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glProvokingVertex. Introduced in GL core 3.2
 * \param mode GLenum
 * \return void
 */
STATICINLINE void provoking_vertex(group::vertex_provoking_mode mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProvokingVertex)
    }
    glProvokingVertex(static_cast<GLenum>(mode));
    detail::error_check("ProvokingVertex"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glClientWaitSync. Introduced in GL core 3.2
 * \param sync GLsync
 * \param flags GLbitfield
 * \param timeout GLuint64
 * \return SyncStatus
 */
STATICINLINE GLenum
    client_wait_sync(GLsync sync, group::sync_object_mask flags, u64 timeout)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClientWaitSync)
    }
    auto out = glClientWaitSync(sync, static_cast<GLenum>(flags), timeout);
    detail::error_check("ClientWaitSync"sv);
    return out;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glDeleteSync. Introduced in GL core 3.2
 * \param sync GLsync
 * \return void
 */
STATICINLINE void delete_sync(GLsync sync)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteSync)
    }
    glDeleteSync(sync);
    detail::error_check("DeleteSync"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glFenceSync. Introduced in GL core 3.2
 * \param condition GLenum
 * \param flags GLbitfield
 * \return sync
 */
STATICINLINE GLsync fence_sync(
    group::sync_condition condition, group::sync_behavior_flags flags)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FenceSync)
    }
    auto out =
        glFenceSync(static_cast<GLenum>(condition), static_cast<GLenum>(flags));
    detail::error_check("FenceSync"sv);
    return out;
}

template<class span_i64>
requires(
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_i64> &&
    std::is_same_v<
        std::decay_t<typename span_i64::value_type>,
        std::decay_t<i64>>)
/*!
 * \brief Wraps around glGetInteger64v. Introduced in GL core 3.2
 * \param pname GLenum
 * \param data GLint64 *
 * \return void
 */
STATICINLINE void get_integer64v(group::get_prop pname, span_i64 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetInteger64v)
    }
    glGetInteger64v(
        static_cast<GLenum>(pname),
        data.size() ? reinterpret_cast<GLint64*>(data.data()) : nullptr);
    detail::error_check("GetInteger64v"sv);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetSynciv. Introduced in GL core 3.2
 * \param sync GLsync
 * \param pname GLenum
 * \param count GLsizei
 * \param length GLsizei *
 * \param values GLint *
 * \return void
 */
STATICINLINE void get_synciv(
    GLsync sync, group::sync_parameter_name pname, i32& length, span_i32 values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSynciv)
    }
    glGetSynciv(
        sync,
        static_cast<GLenum>(pname),
        values.size(),
        &length,
        values.size() ? reinterpret_cast<GLint*>(values.data()) : nullptr);
    detail::error_check("GetSynciv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glIsSync. Introduced in GL core 3.2
 * \param sync GLsync
 * \return Boolean
 */
STATICINLINE GLboolean is_sync(GLsync sync)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsSync)
    }
    auto out = glIsSync(sync);
    detail::error_check("IsSync"sv);
    return out;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glWaitSync. Introduced in GL core 3.2
 * \param sync GLsync
 * \param flags GLbitfield
 * \param timeout GLuint64
 * \return void
 */
STATICINLINE
    void wait_sync(GLsync sync, group::sync_behavior_flags flags, u64 timeout)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WaitSync)
    }
    glWaitSync(sync, static_cast<GLenum>(flags), timeout);
    detail::error_check("WaitSync"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glFramebufferTexture. Introduced in GL core 3.2
 * \param target GLenum
 * \param attachment GLenum
 * \param texture GLuint
 * \param level GLint
 * \return void
 */
STATICINLINE void framebuffer_texture(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    u32                           texture,
    i32                           level)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTexture)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glFramebufferTexture(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        texture,
        level);
    detail::error_check("FramebufferTexture"sv);
}

template<class span_i64>
requires(
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_i64> &&
    std::is_same_v<
        std::decay_t<typename span_i64::value_type>,
        std::decay_t<i64>>)
/*!
 * \brief Wraps around glGetBufferParameteri64v. Introduced in GL core 3.2
 * \param target GLenum
 * \param pname GLenum
 * \param params GLint64 *
 * \return void
 */
STATICINLINE void get_buffer_parameteri64v(
    group::buffer_target_arb target,
    group::buffer_prop_arb   pname,
    span_i64                 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetBufferParameteri64v)
    }
    glGetBufferParameteri64v(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint64*>(params.data()) : nullptr);
    detail::error_check("GetBufferParameteri64v"sv);
}

template<class span_i64>
requires(
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_i64> &&
    std::is_same_v<
        std::decay_t<typename span_i64::value_type>,
        std::decay_t<i64>>)
/*!
 * \brief Wraps around glGetInteger64i_v. Introduced in GL core 3.2
 * \param target GLenum
 * \param index GLuint
 * \param data GLint64 *
 * \return void
 */
STATICINLINE
    void get_integer64i_v(group::get_prop target, u32 index, span_i64 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetInteger64i_v)
    }
    glGetInteger64i_v(
        static_cast<GLenum>(target),
        index,
        data.size() ? reinterpret_cast<GLint64*>(data.data()) : nullptr);
    detail::error_check("GetInteger64i_v"sv);
}

template<class span_f32>
requires(
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<
        std::decay_t<typename span_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Wraps around glGetMultisamplefv. Introduced in GL core 3.2
 * \param pname GLenum
 * \param index GLuint
 * \param val GLfloat *
 * \return void
 */
STATICINLINE void get_multisamplefv(
    group::get_multisample_prop_nv pname, u32 index, span_f32 val)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMultisamplefv)
    }
    glGetMultisamplefv(
        static_cast<GLenum>(pname),
        index,
        val.size() ? reinterpret_cast<GLfloat*>(val.data()) : nullptr);
    detail::error_check("GetMultisamplefv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glSampleMaski. Introduced in GL core 3.2
 * \param maskNumber GLuint
 * \param mask GLbitfield
 * \return void
 */
STATICINLINE void sample_maski(u32 maskNumber, GLbitfield mask)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SampleMaski)
    }
    glSampleMaski(maskNumber, mask);
    detail::error_check("SampleMaski"sv);
}

template<class size_2_i32>
requires(
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Size2D<size_2_i32, i32>)
/*!
 * \brief Wraps around glTexImage2DMultisample. Introduced in GL core 3.2
 * \param target GLenum
 * \param samples GLsizei
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param fixedsamplelocations GLboolean
 * \return void
 */
STATICINLINE void tex_image_2d_multisample(
    group::texture_target  target,
    i32                    samples,
    group::internal_format internalformat,
    size_2_i32 const&      width,
    bool                   fixedsamplelocations)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexImage2DMultisample)
    }
    glTexImage2DMultisample(
        static_cast<GLenum>(target),
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        fixedsamplelocations);
    detail::error_check("TexImage2DMultisample"sv);
}

template<class size_3_i32>
requires(
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Size2D<size_3_i32, i32>)
/*!
 * \brief Wraps around glTexImage3DMultisample. Introduced in GL core 3.2
 * \param target GLenum
 * \param samples GLsizei
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
 * \param fixedsamplelocations GLboolean
 * \return void
 */
STATICINLINE void tex_image_3d_multisample(
    group::texture_target  target,
    i32                    samples,
    group::internal_format internalformat,
    size_3_i32 const&      width,
    bool                   fixedsamplelocations)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexImage3DMultisample)
    }
    glTexImage3DMultisample(
        static_cast<GLenum>(target),
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        width[2],
        fixedsamplelocations);
    detail::error_check("TexImage3DMultisample"sv);
}

#endif // GL_VERSION_3_2
