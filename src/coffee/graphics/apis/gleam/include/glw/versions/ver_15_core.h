#ifdef GL_VERSION_1_5
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 5>>)
/*!
 * \brief Wraps around glBeginQuery. Introduced in GL core 1.5
 * \param target GLenum
 * \param id GLuint
 * \return void
 */
STATICINLINE void begin_query(
    group::query_target target,
    u32                 id,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BeginQuery)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBeginQuery, target, id);
#endif
    glBeginQuery(static_cast<GLenum>(target), id);
    detail::error_check("BeginQuery"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 5>>)
/*!
 * \brief Wraps around glBindBuffer. Introduced in GL core 1.5
 * \param target GLenum
 * \param buffer GLuint
 * \return void
 */
STATICINLINE void bind_buffer(
    group::buffer_target_arb target,
    u32                      buffer,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindBuffer)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBindBuffer, target, buffer);
#endif
    glBindBuffer(static_cast<GLenum>(target), buffer);
    detail::error_check("BindBuffer"sv, check_errors);
}

template<class span_const_void>
requires(
    MinimumVersion<Current, Version<1, 5>> && concepts::span<span_const_void>)
/*!
 * \brief Wraps around glBufferData. Introduced in GL core 1.5
 * \param target GLenum
 * \param size GLsizeiptr
 * \param data const void *
 * \param usage GLenum
 * \return void
 */
STATICINLINE void buffer_data(
    group::buffer_target_arb target,
    span_const_void const&   data,
    group::buffer_usage_arb  usage,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BufferData)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glBufferData,
        gsl::span<const char>(
            reinterpret_cast<const char*>(data.data()), data.size_bytes()),
        target,
        usage);
#endif
    glBufferData(
        static_cast<GLenum>(target),
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr,
        static_cast<GLenum>(usage));
    detail::error_check("BufferData"sv, check_errors);
}

template<class span_const_void>
requires(
    MinimumVersion<Current, Version<1, 5>> && concepts::span<span_const_void>)
/*!
 * \brief Wraps around glBufferSubData. Introduced in GL core 1.5
 * \param target GLenum
 * \param offset GLintptr
 * \param size GLsizeiptr
 * \param data const void *
 * \return void
 */
STATICINLINE void buffer_sub_data(
    group::buffer_target_arb target,
    GLintptr                 offset,
    span_const_void const&   data,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BufferSubData)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glBufferSubData,
        gsl::span<const char>(
            reinterpret_cast<const char*>(data.data()), data.size_bytes()),
        target,
        offset);
#endif
    glBufferSubData(
        static_cast<GLenum>(target),
        offset,
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("BufferSubData"sv, check_errors);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<1, 5>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glDeleteBuffers. Introduced in GL core 1.5
 * \param n GLsizei
 * \param buffers const GLuint *
 * \return void
 */
STATICINLINE void delete_buffers(
    span_const_u32 const& buffers, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteBuffers)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glDeleteBuffers,
        gsl::span<const char>(
            reinterpret_cast<const char*>(buffers.data()),
            buffers.size_bytes()));
#endif
    glDeleteBuffers(
        buffers.size(),
        buffers.size() ? reinterpret_cast<const GLuint*>(buffers.data())
                       : nullptr);
    detail::error_check("DeleteBuffers"sv, check_errors);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<1, 5>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glDeleteQueries. Introduced in GL core 1.5
 * \param n GLsizei
 * \param ids const GLuint *
 * \return void
 */
STATICINLINE void delete_queries(
    span_const_u32 const& ids, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteQueries)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glDeleteQueries,
        gsl::span<const char>(
            reinterpret_cast<const char*>(ids.data()), ids.size_bytes()));
#endif
    glDeleteQueries(
        ids.size(),
        ids.size() ? reinterpret_cast<const GLuint*>(ids.data()) : nullptr);
    detail::error_check("DeleteQueries"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 5>>)
/*!
 * \brief Wraps around glEndQuery. Introduced in GL core 1.5
 * \param target GLenum
 * \return void
 */
STATICINLINE void end_query(
    group::query_target target, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EndQuery)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glEndQuery, target);
#endif
    glEndQuery(static_cast<GLenum>(target));
    detail::error_check("EndQuery"sv, check_errors);
}

template<class span_u32>
requires(
    MinimumVersion<Current, Version<1, 5>> && concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glGenBuffers. Introduced in GL core 1.5
 * \param n GLsizei
 * \param buffers GLuint *
 * \return void
 */
STATICINLINE void gen_buffers(
    span_u32 buffers, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenBuffers)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGenBuffers,
        gsl::span<char>(
            reinterpret_cast<char*>(buffers.data()), buffers.size_bytes()));
#endif
    glGenBuffers(
        buffers.size(),
        buffers.size() ? reinterpret_cast<GLuint*>(buffers.data()) : nullptr);
    detail::error_check("GenBuffers"sv, check_errors);
}

template<class span_u32>
requires(
    MinimumVersion<Current, Version<1, 5>> && concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glGenQueries. Introduced in GL core 1.5
 * \param n GLsizei
 * \param ids GLuint *
 * \return void
 */
STATICINLINE void gen_queries(
    span_u32 ids, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenQueries)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGenQueries,
        gsl::span<char>(reinterpret_cast<char*>(ids.data()), ids.size_bytes()));
#endif
    glGenQueries(
        ids.size(),
        ids.size() ? reinterpret_cast<GLuint*>(ids.data()) : nullptr);
    detail::error_check("GenQueries"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<1, 5>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetBufferParameteriv. Introduced in GL core 1.5
 * \param target GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_buffer_parameter(
    group::buffer_target_arb target,
    group::buffer_prop_arb   pname,
    span_i32                 params,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetBufferParameteriv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetBufferParameteriv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glGetBufferParameteriv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetBufferParameteriv"sv, check_errors);
}

template<class span_void>
requires(MinimumVersion<Current, Version<1, 5>> && concepts::span<span_void>)
/*!
 * \brief Wraps around glGetBufferPointerv. Introduced in GL core 1.5
 * \param target GLenum
 * \param pname GLenum
 * \param params void **
 * \return void
 */
STATICINLINE void get_buffer_pointerv(
    group::buffer_target_arb       target,
    group::buffer_pointer_name_arb pname,
    span_void                      params,
    error_check                    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetBufferPointerv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetBufferPointerv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glGetBufferPointerv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<void**>(params.data()) : nullptr);
    detail::error_check("GetBufferPointerv"sv, check_errors);
}

template<class span_void>
requires(MinimumVersion<Current, Version<1, 5>> && concepts::span<span_void>)
/*!
 * \brief Wraps around glGetBufferSubData. Introduced in GL core 1.5
 * \param target GLenum
 * \param offset GLintptr
 * \param size GLsizeiptr
 * \param data void *
 * \return void
 */
STATICINLINE void get_buffer_sub_data(
    group::buffer_target_arb target,
    GLintptr                 offset,
    span_void                data,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetBufferSubData)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetBufferSubData,
        gsl::span<char>(
            reinterpret_cast<char*>(data.data()), data.size_bytes()),
        target,
        offset);
#endif
    glGetBufferSubData(
        static_cast<GLenum>(target),
        offset,
        data.size() * sizeof(typename std::decay_t<span_void>::value_type),
        data.size() ? reinterpret_cast<void*>(data.data()) : nullptr);
    detail::error_check("GetBufferSubData"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<1, 5>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetQueryObjectiv. Introduced in GL core 1.5
 * \param id GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_query_objectiv(
    u32                                id,
    group::query_object_parameter_name pname,
    span_i32                           params,
    error_check                        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryObjectiv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetQueryObjectiv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        id,
        pname);
#endif
    glGetQueryObjectiv(
        id,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetQueryObjectiv"sv, check_errors);
}

template<class span_u32>
requires(
    MinimumVersion<Current, Version<1, 5>> && concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glGetQueryObjectuiv. Introduced in GL core 1.5
 * \param id GLuint
 * \param pname GLenum
 * \param params GLuint *
 * \return void
 */
STATICINLINE void get_query_objectuiv(
    u32                                id,
    group::query_object_parameter_name pname,
    span_u32                           params,
    error_check                        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryObjectuiv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetQueryObjectuiv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        id,
        pname);
#endif
    glGetQueryObjectuiv(
        id,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetQueryObjectuiv"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<1, 5>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetQueryiv. Introduced in GL core 1.5
 * \param target GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_queryiv(
    group::query_target         target,
    group::query_parameter_name pname,
    span_i32                    params,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryiv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetQueryiv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glGetQueryiv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetQueryiv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 5>>)
/*!
 * \brief Wraps around glIsBuffer. Introduced in GL core 1.5
 * \param buffer GLuint
 * \return GLboolean
 */
STATICINLINE GLboolean
is_buffer(u32 buffer, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsBuffer)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glIsBuffer, buffer);
#endif
    auto out = glIsBuffer(buffer);
    detail::error_check("IsBuffer"sv, check_errors);
    return out;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 5>>)
/*!
 * \brief Wraps around glIsQuery. Introduced in GL core 1.5
 * \param id GLuint
 * \return GLboolean
 */
STATICINLINE GLboolean
is_query(u32 id, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsQuery)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glIsQuery, id);
#endif
    auto out = glIsQuery(id);
    detail::error_check("IsQuery"sv, check_errors);
    return out;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 5>>)
/*!
 * \brief Wraps around glMapBuffer. Introduced in GL core 1.5
 * \param target GLenum
 * \param access GLenum
 * \return void *
 */
STATICINLINE void* map_buffer(
    group::buffer_target_arb target,
    group::buffer_access_arb access,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MapBuffer)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMapBuffer, target, access);
#endif
    auto out =
        glMapBuffer(static_cast<GLenum>(target), static_cast<GLenum>(access));
    detail::error_check("MapBuffer"sv, check_errors);
    return out;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 5>>)
/*!
 * \brief Wraps around glUnmapBuffer. Introduced in GL core 1.5
 * \param target GLenum
 * \return GLboolean
 */
STATICINLINE GLboolean unmap_buffer(
    group::buffer_target_arb target, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UnmapBuffer)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glUnmapBuffer, target);
#endif
    auto out = glUnmapBuffer(static_cast<GLenum>(target));
    detail::error_check("UnmapBuffer"sv, check_errors);
    return out;
}

#endif // GL_VERSION_1_5
