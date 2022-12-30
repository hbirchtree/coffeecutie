#pragma once

#ifdef GL_ARB_vertex_buffer_object
#include "../enums/BufferPNameARB.h"
#include "../enums/BufferPointerNameARB.h"
namespace gl::arb::vertex_buffer_object {
using gl::group::buffer_pointer_name_arb;
using gl::group::buffer_prop_arb;
namespace values {
constexpr libc_types::u32 array_buffer                         = 0x8892;
constexpr libc_types::u32 element_array_buffer                 = 0x8893;
constexpr libc_types::u32 array_buffer_binding                 = 0x8894;
constexpr libc_types::u32 element_array_buffer_binding         = 0x8895;
constexpr libc_types::u32 vertex_array_buffer_binding          = 0x8896;
constexpr libc_types::u32 normal_array_buffer_binding          = 0x8897;
constexpr libc_types::u32 color_array_buffer_binding           = 0x8898;
constexpr libc_types::u32 index_array_buffer_binding           = 0x8899;
constexpr libc_types::u32 texture_coord_array_buffer_binding   = 0x889A;
constexpr libc_types::u32 edge_flag_array_buffer_binding       = 0x889B;
constexpr libc_types::u32 secondary_color_array_buffer_binding = 0x889C;
constexpr libc_types::u32 fog_coordinate_array_buffer_binding  = 0x889D;
constexpr libc_types::u32 weight_array_buffer_binding          = 0x889E;
constexpr libc_types::u32 vertex_attrib_array_buffer_binding   = 0x889F;
constexpr libc_types::u32 read_only                            = 0x88B8;
constexpr libc_types::u32 write_only                           = 0x88B9;
constexpr libc_types::u32 read_write                           = 0x88BA;
constexpr libc_types::u32 stream_draw                          = 0x88E0;
constexpr libc_types::u32 stream_read                          = 0x88E1;
constexpr libc_types::u32 stream_copy                          = 0x88E2;
constexpr libc_types::u32 static_draw                          = 0x88E4;
constexpr libc_types::u32 static_read                          = 0x88E5;
constexpr libc_types::u32 static_copy                          = 0x88E6;
constexpr libc_types::u32 dynamic_draw                         = 0x88E8;
constexpr libc_types::u32 dynamic_read                         = 0x88E9;
constexpr libc_types::u32 dynamic_copy                         = 0x88EA;
} // namespace values
/*!
 * \brief Part of GL_ARB_vertex_buffer_object
 * \param target GLenum
 * \param buffer GLuint
 * \return void
 */
STATICINLINE void bind_buffer(group::buffer_target_arb target, u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindBufferARB)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glBindBufferARB(static_cast<GLenum>(target), buffer);
    detail::error_check("BindBufferARB"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
/*!
 * \brief Part of GL_ARB_vertex_buffer_object
 * \param target GLenum
 * \param size GLsizeiptrARB
 * \param data const void *
 * \param usage GLenum
 * \return void
 */
STATICINLINE void buffer_data(
    group::buffer_target_arb target,
    span_const_void const&   data,
    group::buffer_usage_arb  usage)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BufferDataARB)
    }
    glBufferDataARB(
        static_cast<GLenum>(target),
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr,
        static_cast<GLenum>(usage));
    detail::error_check("BufferDataARB"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
/*!
 * \brief Part of GL_ARB_vertex_buffer_object
 * \param target GLenum
 * \param offset GLintptrARB
 * \param size GLsizeiptrARB
 * \param data const void *
 * \return void
 */
STATICINLINE void buffer_sub_data(
    group::buffer_target_arb target,
    GLintptrARB              offset,
    span_const_void const&   data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BufferSubDataARB)
    }
    glBufferSubDataARB(
        static_cast<GLenum>(target),
        offset,
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("BufferSubDataARB"sv);
}

template<class span_const_u32>
requires(
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_ARB_vertex_buffer_object
 * \param n GLsizei
 * \param buffers const GLuint *
 * \return void
 */
STATICINLINE void delete_buffers(span_const_u32 const& buffers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteBuffersARB)
    }
    glDeleteBuffersARB(
        buffers.size(),
        buffers.size() ? reinterpret_cast<const GLuint*>(buffers.data())
                       : nullptr);
    detail::error_check("DeleteBuffersARB"sv);
}

template<class span_u32>
requires(
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_ARB_vertex_buffer_object
 * \param n GLsizei
 * \param buffers GLuint *
 * \return void
 */
STATICINLINE void gen_buffers(span_u32 buffers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenBuffersARB)
    }
    glGenBuffersARB(
        buffers.size(),
        buffers.size() ? reinterpret_cast<GLuint*>(buffers.data()) : nullptr);
    detail::error_check("GenBuffersARB"sv);
}

template<class span_i32>
requires(
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_ARB_vertex_buffer_object
 * \param target GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_buffer_parameter(
    group::buffer_target_arb target,
    group::buffer_prop_arb   pname,
    span_i32                 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetBufferParameterivARB)
    }
    glGetBufferParameterivARB(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetBufferParameterivARB"sv);
}

template<class span_void>
requires(semantic::concepts::Span<span_void>)
/*!
 * \brief Part of GL_ARB_vertex_buffer_object
 * \param target GLenum
 * \param pname GLenum
 * \param params void **
 * \return void
 */
STATICINLINE void get_buffer_pointerv(
    group::buffer_target_arb       target,
    group::buffer_pointer_name_arb pname,
    span_void                      params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetBufferPointervARB)
    }
    glGetBufferPointervARB(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<void**>(params.data()) : nullptr);
    detail::error_check("GetBufferPointervARB"sv);
}

template<class span_void>
requires(semantic::concepts::Span<span_void>)
/*!
 * \brief Part of GL_ARB_vertex_buffer_object
 * \param target GLenum
 * \param offset GLintptrARB
 * \param size GLsizeiptrARB
 * \param data void *
 * \return void
 */
STATICINLINE void get_buffer_sub_data(
    group::buffer_target_arb target, GLintptrARB offset, span_void data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetBufferSubDataARB)
    }
    glGetBufferSubDataARB(
        static_cast<GLenum>(target),
        offset,
        data.size() * sizeof(typename std::decay_t<span_void>::value_type),
        data.size() ? reinterpret_cast<void*>(data.data()) : nullptr);
    detail::error_check("GetBufferSubDataARB"sv);
}

/*!
 * \brief Part of GL_ARB_vertex_buffer_object
 * \param buffer GLuint
 * \return Boolean
 */
STATICINLINE GLboolean is_buffer(u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsBufferARB)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    auto out = glIsBufferARB(buffer);
    detail::error_check("IsBufferARB"sv);
    return out;
}

/*!
 * \brief Part of GL_ARB_vertex_buffer_object
 * \param target GLenum
 * \param access GLenum
 * \return void *
 */
STATICINLINE void* map_buffer(
    group::buffer_target_arb target, group::buffer_access_arb access)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MapBufferARB)
    }
    auto out = glMapBufferARB(
        static_cast<GLenum>(target), static_cast<GLenum>(access));
    detail::error_check("MapBufferARB"sv);
    return out;
}

/*!
 * \brief Part of GL_ARB_vertex_buffer_object
 * \param target GLenum
 * \return Boolean
 */
STATICINLINE GLboolean unmap_buffer(group::buffer_target_arb target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UnmapBufferARB)
    }
    auto out = glUnmapBufferARB(static_cast<GLenum>(target));
    detail::error_check("UnmapBufferARB"sv);
    return out;
}

} // namespace gl::arb::vertex_buffer_object
#endif // GL_ARB_vertex_buffer_object
namespace gl::arb::vertex_buffer_object {
constexpr auto name = "GL_ARB_vertex_buffer_object";
}
