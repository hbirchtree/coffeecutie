#pragma once

#ifdef GL_NV_memory_object_sparse
namespace gl::nv::memory_object_sparse {
namespace values {
} // namespace values
/*!
 * \brief Part of GL_NV_memory_object_sparse
 * \param target GLenum
 * \param offset GLintptr
 * \param size GLsizeiptr
 * \param memory GLuint
 * \param memOffset GLuint64
 * \param commit GLboolean
 * \return void
 */
STATICINLINE void buffer_page_commitment_mem(
    group::buffer_storage_target target,
    GLintptr                     offset,
    GLsizeiptr                   size,
    u32                          memory,
    u64                          memOffset,
    bool                         commit)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BufferPageCommitmentMemNV)
    }
    glBufferPageCommitmentMemNV(
        static_cast<GLenum>(target), offset, size, memory, memOffset, commit);
    detail::error_check("BufferPageCommitmentMemNV"sv);
}

template<class size_3_i32, class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>&&
             semantic::concepts::Size2D<size_3_i32, i32>)
    /*!
     * \brief Part of GL_NV_memory_object_sparse
     * \param target GLenum
     * \param layer GLint
     * \param level GLint
     * \param xoffset GLint
     * \param yoffset GLint
     * \param zoffset GLint
     * \param width GLsizei
     * \param height GLsizei
     * \param depth GLsizei
     * \param memory GLuint
     * \param offset GLuint64
     * \param commit GLboolean
     * \return void
     */
    STATICINLINE void tex_page_commitment_mem(
        group::texture_target target,
        i32                   layer,
        i32                   level,
        vec_3_i32 const&      xoffset,
        size_3_i32 const&     width,
        u32                   memory,
        u64                   offset,
        bool                  commit)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexPageCommitmentMemNV)
    }
    glTexPageCommitmentMemNV(
        static_cast<GLenum>(target),
        layer,
        level,
        xoffset[0],
        xoffset[1],
        xoffset[2],
        width[0],
        width[1],
        width[2],
        memory,
        offset,
        commit);
    detail::error_check("TexPageCommitmentMemNV"sv);
}

/*!
 * \brief Part of GL_NV_memory_object_sparse
 * \param buffer GLuint
 * \param offset GLintptr
 * \param size GLsizeiptr
 * \param memory GLuint
 * \param memOffset GLuint64
 * \param commit GLboolean
 * \return void
 */
STATICINLINE void named_buffer_page_commitment_mem(
    u32        buffer,
    GLintptr   offset,
    GLsizeiptr size,
    u32        memory,
    u64        memOffset,
    bool       commit)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedBufferPageCommitmentMemNV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glNamedBufferPageCommitmentMemNV(
        buffer, offset, size, memory, memOffset, commit);
    detail::error_check("NamedBufferPageCommitmentMemNV"sv);
}

template<class size_3_i32, class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>&&
             semantic::concepts::Size2D<size_3_i32, i32>)
    /*!
     * \brief Part of GL_NV_memory_object_sparse
     * \param texture GLuint
     * \param layer GLint
     * \param level GLint
     * \param xoffset GLint
     * \param yoffset GLint
     * \param zoffset GLint
     * \param width GLsizei
     * \param height GLsizei
     * \param depth GLsizei
     * \param memory GLuint
     * \param offset GLuint64
     * \param commit GLboolean
     * \return void
     */
    STATICINLINE void texture_page_commitment_mem(
        u32               texture,
        i32               layer,
        i32               level,
        vec_3_i32 const&  xoffset,
        size_3_i32 const& width,
        u32               memory,
        u64               offset,
        bool              commit)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexturePageCommitmentMemNV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTexturePageCommitmentMemNV(
        texture,
        layer,
        level,
        xoffset[0],
        xoffset[1],
        xoffset[2],
        width[0],
        width[1],
        width[2],
        memory,
        offset,
        commit);
    detail::error_check("TexturePageCommitmentMemNV"sv);
}

} // namespace gl::nv::memory_object_sparse
#endif // GL_NV_memory_object_sparse
namespace gl::nv::memory_object_sparse {
constexpr auto name = "GL_NV_memory_object_sparse";
}
