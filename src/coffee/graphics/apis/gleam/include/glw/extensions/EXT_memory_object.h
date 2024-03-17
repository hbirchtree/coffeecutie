#pragma once

#ifdef GL_EXT_memory_object
#include "../enums/GetPName.h"
#include "../enums/MemoryObjectParameterName.h"
#include "../enums/SpecialNumbers.h"
#include "../enums/TextureParameterName.h"

namespace gl::ext::memory_object {
using gl::group::get_prop;
using gl::group::memory_object_parameter_name;
using gl::group::special_numbers;
using gl::group::texture_parameter_name;

namespace values {
constexpr u32 num_tiling_types = 0x9582;
constexpr u32 tiling_types     = 0x9583;
constexpr u32 optimal_tiling   = 0x9584;
constexpr u32 linear_tiling    = 0x9585;
} // namespace values

/*!
 * \brief Part of GL_EXT_memory_object
 * \param target GLenum
 * \param size GLsizeiptr
 * \param memory GLuint
 * \param offset GLuint64
 * \return void
 */
STATICINLINE void buffer_storage_mem(
    group::buffer_target_arb target,
    GLsizeiptr               size,
    u32                      memory,
    u64                      offset,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BufferStorageMemEXT)
    }
    glBufferStorageMemEXT(static_cast<GLenum>(target), size, memory, offset);
    detail::error_check("BufferStorageMemEXT"sv, check_errors);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_EXT_memory_object
 * \param n GLsizei
 * \param memoryObjects GLuint *
 * \return void
 */
STATICINLINE void create_memory_objects(
    i32 n, span_u32 memoryObjects, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateMemoryObjectsEXT)
    }
    glCreateMemoryObjectsEXT(
        n,
        memoryObjects.size() ? reinterpret_cast<GLuint*>(memoryObjects.data())
                             : nullptr);
    detail::error_check("CreateMemoryObjectsEXT"sv, check_errors);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_EXT_memory_object
 * \param n GLsizei
 * \param memoryObjects const GLuint *
 * \return void
 */
STATICINLINE void delete_memory_objects(
    span_const_u32 const& memoryObjects,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteMemoryObjectsEXT)
    }
    glDeleteMemoryObjectsEXT(
        memoryObjects.size(),
        memoryObjects.size()
            ? reinterpret_cast<const GLuint*>(memoryObjects.data())
            : nullptr);
    detail::error_check("DeleteMemoryObjectsEXT"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_memory_object
 * \param memoryObject GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_memory_object_parameter(
    u32                                 memoryObject,
    group::memory_object_parameter_name pname,
    span_i32                            params,
    error_check                         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMemoryObjectParameterivEXT)
    }
    glGetMemoryObjectParameterivEXT(
        memoryObject,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetMemoryObjectParameterivEXT"sv, check_errors);
}

template<class span_u8>
requires(
    concepts::span<span_u8> &&
    std::
        is_same_v<std::decay_t<typename span_u8::value_type>, std::decay_t<u8>>)
/*!
 * \brief Part of GL_EXT_memory_object
 * \param pname GLenum
 * \param data GLubyte *
 * \return void
 */
STATICINLINE void get_unsigned_bytev(
    group::get_prop pname,
    span_u8         data,
    error_check     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUnsignedBytevEXT)
    }
    glGetUnsignedBytevEXT(
        static_cast<GLenum>(pname),
        data.size() ? reinterpret_cast<GLubyte*>(data.data()) : nullptr);
    detail::error_check("GetUnsignedBytevEXT"sv, check_errors);
}

template<class span_u8>
requires(
    concepts::span<span_u8> &&
    std::
        is_same_v<std::decay_t<typename span_u8::value_type>, std::decay_t<u8>>)
/*!
 * \brief Part of GL_EXT_memory_object
 * \param target GLenum
 * \param index GLuint
 * \param data GLubyte *
 * \return void
 */
STATICINLINE void get_unsigned_bytei_v(
    GLenum      target,
    u32         index,
    span_u8     data,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUnsignedBytei_vEXT)
    }
    glGetUnsignedBytei_vEXT(
        target,
        index,
        data.size() ? reinterpret_cast<GLubyte*>(data.data()) : nullptr);
    detail::error_check("GetUnsignedBytei_vEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_memory_object
 * \param memoryObject GLuint
 * \return Boolean
 */
STATICINLINE bool is_memory_object(
    u32 memoryObject, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsMemoryObjectEXT)
    }
    auto out = glIsMemoryObjectEXT(memoryObject);
    detail::error_check("IsMemoryObjectEXT"sv, check_errors);
    return out == GL_TRUE ? true : false;
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_memory_object
 * \param memoryObject GLuint
 * \param pname GLenum
 * \param params const GLint *
 * \return void
 */
STATICINLINE void memory_object_parameter(
    u32                                 memoryObject,
    group::memory_object_parameter_name pname,
    span_const_i32 const&               params,
    error_check                         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MemoryObjectParameterivEXT)
    }
    glMemoryObjectParameterivEXT(
        memoryObject,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("MemoryObjectParameterivEXT"sv, check_errors);
}

template<class size_2_i32>
requires(concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Part of GL_EXT_memory_object
 * \param target GLenum
 * \param levels GLsizei
 * \param internalFormat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param memory GLuint
 * \param offset GLuint64
 * \return void
 */
STATICINLINE void tex_storage_mem_2d(
    group::texture_target        target,
    i32                          levels,
    group::sized_internal_format internalFormat,
    size_2_i32 const&            width,
    u32                          memory,
    u64                          offset,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexStorageMem2DEXT)
    }
    glTexStorageMem2DEXT(
        static_cast<GLenum>(target),
        levels,
        static_cast<GLenum>(internalFormat),
        width[0],
        width[1],
        memory,
        offset);
    detail::error_check("TexStorageMem2DEXT"sv, check_errors);
}

template<class size_2_i32>
requires(concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Part of GL_EXT_memory_object
 * \param target GLenum
 * \param samples GLsizei
 * \param internalFormat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param fixedSampleLocations GLboolean
 * \param memory GLuint
 * \param offset GLuint64
 * \return void
 */
STATICINLINE void tex_storage_mem_2d_multisample(
    group::texture_target        target,
    i32                          samples,
    group::sized_internal_format internalFormat,
    size_2_i32 const&            width,
    bool                         fixedSampleLocations,
    u32                          memory,
    u64                          offset,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexStorageMem2DMultisampleEXT)
    }
    glTexStorageMem2DMultisampleEXT(
        static_cast<GLenum>(target),
        samples,
        static_cast<GLenum>(internalFormat),
        width[0],
        width[1],
        fixedSampleLocations,
        memory,
        offset);
    detail::error_check("TexStorageMem2DMultisampleEXT"sv, check_errors);
}

template<class size_3_i32>
requires(concepts::size_2d<size_3_i32, i32>)
/*!
 * \brief Part of GL_EXT_memory_object
 * \param target GLenum
 * \param levels GLsizei
 * \param internalFormat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
 * \param memory GLuint
 * \param offset GLuint64
 * \return void
 */
STATICINLINE void tex_storage_mem_3d(
    group::texture_target        target,
    i32                          levels,
    group::sized_internal_format internalFormat,
    size_3_i32 const&            width,
    u32                          memory,
    u64                          offset,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexStorageMem3DEXT)
    }
    glTexStorageMem3DEXT(
        static_cast<GLenum>(target),
        levels,
        static_cast<GLenum>(internalFormat),
        width[0],
        width[1],
        width[2],
        memory,
        offset);
    detail::error_check("TexStorageMem3DEXT"sv, check_errors);
}

template<class size_3_i32>
requires(concepts::size_2d<size_3_i32, i32>)
/*!
 * \brief Part of GL_EXT_memory_object
 * \param target GLenum
 * \param samples GLsizei
 * \param internalFormat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
 * \param fixedSampleLocations GLboolean
 * \param memory GLuint
 * \param offset GLuint64
 * \return void
 */
STATICINLINE void tex_storage_mem_3d_multisample(
    group::texture_target        target,
    i32                          samples,
    group::sized_internal_format internalFormat,
    size_3_i32 const&            width,
    bool                         fixedSampleLocations,
    u32                          memory,
    u64                          offset,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexStorageMem3DMultisampleEXT)
    }
    glTexStorageMem3DMultisampleEXT(
        static_cast<GLenum>(target),
        samples,
        static_cast<GLenum>(internalFormat),
        width[0],
        width[1],
        width[2],
        fixedSampleLocations,
        memory,
        offset);
    detail::error_check("TexStorageMem3DMultisampleEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_memory_object
 * \param buffer GLuint
 * \param size GLsizeiptr
 * \param memory GLuint
 * \param offset GLuint64
 * \return void
 */
STATICINLINE void named_buffer_storage_mem(
    u32         buffer,
    GLsizeiptr  size,
    u32         memory,
    u64         offset,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedBufferStorageMemEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glNamedBufferStorageMemEXT(buffer, size, memory, offset);
    detail::error_check("NamedBufferStorageMemEXT"sv, check_errors);
}

template<class size_2_i32>
requires(concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Part of GL_EXT_memory_object
 * \param texture GLuint
 * \param levels GLsizei
 * \param internalFormat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param memory GLuint
 * \param offset GLuint64
 * \return void
 */
STATICINLINE void texture_storage_mem_2d(
    u32                          texture,
    i32                          levels,
    group::sized_internal_format internalFormat,
    size_2_i32 const&            width,
    u32                          memory,
    u64                          offset,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorageMem2DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureStorageMem2DEXT(
        texture,
        levels,
        static_cast<GLenum>(internalFormat),
        width[0],
        width[1],
        memory,
        offset);
    detail::error_check("TextureStorageMem2DEXT"sv, check_errors);
}

template<class size_2_i32>
requires(concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Part of GL_EXT_memory_object
 * \param texture GLuint
 * \param samples GLsizei
 * \param internalFormat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param fixedSampleLocations GLboolean
 * \param memory GLuint
 * \param offset GLuint64
 * \return void
 */
STATICINLINE void texture_storage_mem_2d_multisample(
    u32                          texture,
    i32                          samples,
    group::sized_internal_format internalFormat,
    size_2_i32 const&            width,
    bool                         fixedSampleLocations,
    u32                          memory,
    u64                          offset,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorageMem2DMultisampleEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureStorageMem2DMultisampleEXT(
        texture,
        samples,
        static_cast<GLenum>(internalFormat),
        width[0],
        width[1],
        fixedSampleLocations,
        memory,
        offset);
    detail::error_check("TextureStorageMem2DMultisampleEXT"sv, check_errors);
}

template<class size_3_i32>
requires(concepts::size_2d<size_3_i32, i32>)
/*!
 * \brief Part of GL_EXT_memory_object
 * \param texture GLuint
 * \param levels GLsizei
 * \param internalFormat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
 * \param memory GLuint
 * \param offset GLuint64
 * \return void
 */
STATICINLINE void texture_storage_mem_3d(
    u32                          texture,
    i32                          levels,
    group::sized_internal_format internalFormat,
    size_3_i32 const&            width,
    u32                          memory,
    u64                          offset,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorageMem3DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureStorageMem3DEXT(
        texture,
        levels,
        static_cast<GLenum>(internalFormat),
        width[0],
        width[1],
        width[2],
        memory,
        offset);
    detail::error_check("TextureStorageMem3DEXT"sv, check_errors);
}

template<class size_3_i32>
requires(concepts::size_2d<size_3_i32, i32>)
/*!
 * \brief Part of GL_EXT_memory_object
 * \param texture GLuint
 * \param samples GLsizei
 * \param internalFormat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
 * \param fixedSampleLocations GLboolean
 * \param memory GLuint
 * \param offset GLuint64
 * \return void
 */
STATICINLINE void texture_storage_mem_3d_multisample(
    u32                          texture,
    i32                          samples,
    group::sized_internal_format internalFormat,
    size_3_i32 const&            width,
    bool                         fixedSampleLocations,
    u32                          memory,
    u64                          offset,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorageMem3DMultisampleEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureStorageMem3DMultisampleEXT(
        texture,
        samples,
        static_cast<GLenum>(internalFormat),
        width[0],
        width[1],
        width[2],
        fixedSampleLocations,
        memory,
        offset);
    detail::error_check("TextureStorageMem3DMultisampleEXT"sv, check_errors);
}

#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_EXT_memory_object
 * \param target GLenum
 * \param levels GLsizei
 * \param internalFormat GLenum
 * \param width GLsizei
 * \param memory GLuint
 * \param offset GLuint64
 * \return void
 */
STATICINLINE void tex_storage_mem_1d(
    group::texture_target        target,
    i32                          levels,
    group::sized_internal_format internalFormat,
    i32                          width,
    u32                          memory,
    u64                          offset,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexStorageMem1DEXT)
    }
    glTexStorageMem1DEXT(
        static_cast<GLenum>(target),
        levels,
        static_cast<GLenum>(internalFormat),
        width,
        memory,
        offset);
    detail::error_check("TexStorageMem1DEXT"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_EXT_memory_object
 * \param texture GLuint
 * \param levels GLsizei
 * \param internalFormat GLenum
 * \param width GLsizei
 * \param memory GLuint
 * \param offset GLuint64
 * \return void
 */
STATICINLINE void texture_storage_mem_1d(
    u32                          texture,
    i32                          levels,
    group::sized_internal_format internalFormat,
    i32                          width,
    u32                          memory,
    u64                          offset,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorageMem1DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureStorageMem1DEXT(
        texture,
        levels,
        static_cast<GLenum>(internalFormat),
        width,
        memory,
        offset);
    detail::error_check("TextureStorageMem1DEXT"sv, check_errors);
}

#endif
} // namespace gl::ext::memory_object
#endif // GL_EXT_memory_object
namespace gl::ext::memory_object {
constexpr auto name = "GL_EXT_memory_object";
}
