#ifdef GL_VERSION_1_5
/* Introduced in GL core 1.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 5>>) STATICINLINE
    void begin_query(group::query_target target, u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BeginQuery)
    }
    glBeginQuery(static_cast<GLenum>(target), id);
    detail::error_check("BeginQuery"sv);
}

/* Introduced in GL core 1.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 5>>) STATICINLINE
    void bind_buffer(group::buffer_target_arb target, u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindBuffer)
        glIsBuffer(buffer);
    }
    glBindBuffer(static_cast<GLenum>(target), buffer);
    detail::error_check("BindBuffer"sv);
}

/* Introduced in GL core 1.5 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<1, 5>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void buffer_data(
        group::buffer_target_arb target,
        span_const_void const&   data,
        group::buffer_usage_arb  usage)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BufferData)
    }
    glBufferData(
        static_cast<GLenum>(target),
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr,
        static_cast<GLenum>(usage));
    detail::error_check("BufferData"sv);
}

/* Introduced in GL core 1.5 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<1, 5>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void buffer_sub_data(
        group::buffer_target_arb target,
        GLintptr                 offset,
        span_const_void const&   data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BufferSubData)
    }
    glBufferSubData(
        static_cast<GLenum>(target),
        offset,
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("BufferSubData"sv);
}

/* Introduced in GL core 1.5 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<1, 5>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void delete_buffers(span_const_u32 const& buffers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteBuffers)
    }
    glDeleteBuffers(
        buffers.size(),
        buffers.size() ? reinterpret_cast<const GLuint*>(buffers.data())
                       : nullptr);
    detail::error_check("DeleteBuffers"sv);
}

/* Introduced in GL core 1.5 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<1, 5>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void delete_queries(span_const_u32 const& ids)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteQueries)
    }
    glDeleteQueries(
        ids.size(),
        ids.size() ? reinterpret_cast<const GLuint*>(ids.data()) : nullptr);
    detail::error_check("DeleteQueries"sv);
}

/* Introduced in GL core 1.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 5>>) STATICINLINE
    void end_query(group::query_target target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EndQuery)
    }
    glEndQuery(static_cast<GLenum>(target));
    detail::error_check("EndQuery"sv);
}

/* Introduced in GL core 1.5 */
template<class span_u32>
requires(MinimumVersion<Current, Version<1, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void gen_buffers(span_u32 buffers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenBuffers)
    }
    glGenBuffers(
        buffers.size(),
        buffers.size() ? reinterpret_cast<GLuint*>(buffers.data()) : nullptr);
    detail::error_check("GenBuffers"sv);
}

/* Introduced in GL core 1.5 */
template<class span_u32>
requires(MinimumVersion<Current, Version<1, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE void gen_queries(span_u32 ids)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenQueries)
    }
    glGenQueries(
        ids.size(),
        ids.size() ? reinterpret_cast<GLuint*>(ids.data()) : nullptr);
    detail::error_check("GenQueries"sv);
}

/* Introduced in GL core 1.5 */
template<class span_i32>
requires(MinimumVersion<Current, Version<1, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_buffer_parameter(
        group::buffer_target_arb target,
        group::buffer_prop_arb   pname,
        span_i32                 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetBufferParameteriv)
    }
    glGetBufferParameteriv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetBufferParameteriv"sv);
}

/* Introduced in GL core 1.5 */
template<class span_void>
requires(MinimumVersion<Current, Version<1, 5>>&&
             semantic::concepts::Span<span_void>) STATICINLINE
    void get_buffer_pointerv(
        group::buffer_target_arb       target,
        group::buffer_pointer_name_arb pname,
        span_void                      params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetBufferPointerv)
    }
    glGetBufferPointerv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<void**>(params.data()) : nullptr);
    detail::error_check("GetBufferPointerv"sv);
}

/* Introduced in GL core 1.5 */
template<class span_void>
requires(MinimumVersion<Current, Version<1, 5>>&&
             semantic::concepts::Span<span_void>) STATICINLINE
    void get_buffer_sub_data(
        group::buffer_target_arb target, GLintptr offset, span_void data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetBufferSubData)
    }
    glGetBufferSubData(
        static_cast<GLenum>(target),
        offset,
        data.size() * sizeof(typename std::decay_t<span_void>::value_type),
        data.size() ? reinterpret_cast<void*>(data.data()) : nullptr);
    detail::error_check("GetBufferSubData"sv);
}

/* Introduced in GL core 1.5 */
template<class span_i32>
requires(MinimumVersion<Current, Version<1, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_query_objectiv(
        u32 id, group::query_object_parameter_name pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryObjectiv)
    }
    glGetQueryObjectiv(
        id,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetQueryObjectiv"sv);
}

/* Introduced in GL core 1.5 */
template<class span_u32>
requires(MinimumVersion<Current, Version<1, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void get_query_objectuiv(
        u32 id, group::query_object_parameter_name pname, span_u32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryObjectuiv)
    }
    glGetQueryObjectuiv(
        id,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetQueryObjectuiv"sv);
}

/* Introduced in GL core 1.5 */
template<class span_i32>
requires(MinimumVersion<Current, Version<1, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_queryiv(
        group::query_target         target,
        group::query_parameter_name pname,
        span_i32                    params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryiv)
    }
    glGetQueryiv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetQueryiv"sv);
}

/* Introduced in GL core 1.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 5>>) STATICINLINE GLboolean
    is_buffer(u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsBuffer)
        glIsBuffer(buffer);
    }
    auto out = glIsBuffer(buffer);
    detail::error_check("IsBuffer"sv);
    return out;
}

/* Introduced in GL core 1.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 5>>) STATICINLINE GLboolean
    is_query(u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsQuery)
    }
    auto out = glIsQuery(id);
    detail::error_check("IsQuery"sv);
    return out;
}

/* Introduced in GL core 1.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 5>>) STATICINLINE void* map_buffer(
    group::buffer_target_arb target, group::buffer_access_arb access)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MapBuffer)
    }
    auto out =
        glMapBuffer(static_cast<GLenum>(target), static_cast<GLenum>(access));
    detail::error_check("MapBuffer"sv);
    return out;
}

/* Introduced in GL core 1.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 5>>) STATICINLINE GLboolean
    unmap_buffer(group::buffer_target_arb target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UnmapBuffer)
    }
    auto out = glUnmapBuffer(static_cast<GLenum>(target));
    detail::error_check("UnmapBuffer"sv);
    return out;
}

#endif // GL_VERSION_1_5