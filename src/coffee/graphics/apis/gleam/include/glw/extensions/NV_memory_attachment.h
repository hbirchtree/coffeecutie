#pragma once

#ifdef GL_NV_memory_attachment
namespace gl::nv::memory_attachment {
namespace values {
constexpr libc_types::u32 attached_memory_object      = 0x95A4;
constexpr libc_types::u32 attached_memory_offset      = 0x95A5;
constexpr libc_types::u32 memory_attachable_alignment = 0x95A6;
constexpr libc_types::u32 memory_attachable_size      = 0x95A7;
constexpr libc_types::u32 memory_attachable           = 0x95A8;
constexpr libc_types::u32 detached_memory_incarnation = 0x95A9;
constexpr libc_types::u32 detached_textures           = 0x95AA;
constexpr libc_types::u32 detached_buffers            = 0x95AB;
constexpr libc_types::u32 max_detached_textures       = 0x95AC;
constexpr libc_types::u32 max_detached_buffers        = 0x95AD;
} // namespace values
/*!
 * \brief Part of GL_NV_memory_attachment
 * \param target GLenum
 * \param memory GLuint
 * \param offset GLuint64
 * \return void
 */
STATICINLINE void buffer_attach_memory(
    group::buffer_target_arb target, u32 memory, u64 offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BufferAttachMemoryNV)
    }
    glBufferAttachMemoryNV(static_cast<GLenum>(target), memory, offset);
    detail::error_check("BufferAttachMemoryNV"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_NV_memory_attachment
     * \param memory GLuint
     * \param pname GLenum
     * \param first GLint
     * \param count GLsizei
     * \param params GLuint *
     * \return void
     */
    STATICINLINE void get_memory_object_detached_resourcesuiv(
        u32 memory, GLenum pname, i32 first, i32 count, span_u32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMemoryObjectDetachedResourcesuivNV)
    }
    glGetMemoryObjectDetachedResourcesuivNV(
        memory,
        pname,
        first,
        count,
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetMemoryObjectDetachedResourcesuivNV"sv);
}

/*!
 * \brief Part of GL_NV_memory_attachment
 * \param memory GLuint
 * \param pname GLenum
 * \return void
 */
STATICINLINE void reset_memory_object_parameter(u32 memory, GLenum pname)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ResetMemoryObjectParameterNV)
    }
    glResetMemoryObjectParameterNV(memory, pname);
    detail::error_check("ResetMemoryObjectParameterNV"sv);
}

/*!
 * \brief Part of GL_NV_memory_attachment
 * \param target GLenum
 * \param memory GLuint
 * \param offset GLuint64
 * \return void
 */
STATICINLINE void tex_attach_memory(
    group::texture_target target, u32 memory, u64 offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexAttachMemoryNV)
    }
    glTexAttachMemoryNV(static_cast<GLenum>(target), memory, offset);
    detail::error_check("TexAttachMemoryNV"sv);
}

/*!
 * \brief Part of GL_NV_memory_attachment
 * \param buffer GLuint
 * \param memory GLuint
 * \param offset GLuint64
 * \return void
 */
STATICINLINE void named_buffer_attach_memory(u32 buffer, u32 memory, u64 offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedBufferAttachMemoryNV)
        glIsBuffer(buffer);
    }
    glNamedBufferAttachMemoryNV(buffer, memory, offset);
    detail::error_check("NamedBufferAttachMemoryNV"sv);
}

/*!
 * \brief Part of GL_NV_memory_attachment
 * \param texture GLuint
 * \param memory GLuint
 * \param offset GLuint64
 * \return void
 */
STATICINLINE void texture_attach_memory(u32 texture, u32 memory, u64 offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureAttachMemoryNV)
        glIsTexture(texture);
    }
    glTextureAttachMemoryNV(texture, memory, offset);
    detail::error_check("TextureAttachMemoryNV"sv);
}

} // namespace gl::nv::memory_attachment
#endif // GL_NV_memory_attachment
namespace gl::nv::memory_attachment {
constexpr auto name = "GL_NV_memory_attachment";
}
