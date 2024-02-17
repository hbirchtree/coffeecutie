#pragma once

#ifdef GL_EXT_semaphore
#include "../enums/GetPName.h"
#include "../enums/SpecialNumbers.h"
#include "../enums/TextureLayout.h"

namespace gl::ext::semaphore {
using gl::group::get_prop;
using gl::group::special_numbers;
using gl::group::texture_layout;

namespace values {
} // namespace values

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_EXT_semaphore
 * \param n GLsizei
 * \param semaphores const GLuint *
 * \return void
 */
STATICINLINE void delete_semaphores(span_const_u32 const& semaphores)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteSemaphoresEXT)
    }
    glDeleteSemaphoresEXT(
        semaphores.size(),
        semaphores.size() ? reinterpret_cast<const GLuint*>(semaphores.data())
                          : nullptr);
    detail::error_check("DeleteSemaphoresEXT"sv);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_EXT_semaphore
 * \param n GLsizei
 * \param semaphores GLuint *
 * \return void
 */
STATICINLINE void gen_semaphores(span_u32 semaphores)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenSemaphoresEXT)
    }
    glGenSemaphoresEXT(
        semaphores.size(),
        semaphores.size() ? reinterpret_cast<GLuint*>(semaphores.data())
                          : nullptr);
    detail::error_check("GenSemaphoresEXT"sv);
}

template<class span_u64>
requires(
    concepts::span<span_u64> && std::is_same_v<
                                    std::decay_t<typename span_u64::value_type>,
                                    std::decay_t<u64>>)
/*!
 * \brief Part of GL_EXT_semaphore
 * \param semaphore GLuint
 * \param pname GLenum
 * \param params GLuint64 *
 * \return void
 */
STATICINLINE void get_semaphore_parameterui64v(
    u32 semaphore, group::semaphore_parameter_name pname, span_u64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSemaphoreParameterui64vEXT)
    }
    glGetSemaphoreParameterui64vEXT(
        semaphore,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint64*>(params.data()) : nullptr);
    detail::error_check("GetSemaphoreParameterui64vEXT"sv);
}

template<class span_u8>
requires(
    concepts::span<span_u8> &&
    std::
        is_same_v<std::decay_t<typename span_u8::value_type>, std::decay_t<u8>>)
/*!
 * \brief Part of GL_EXT_semaphore
 * \param pname GLenum
 * \param data GLubyte *
 * \return void
 */
STATICINLINE void get_unsigned_bytev(group::get_prop pname, span_u8 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUnsignedBytevEXT)
    }
    glGetUnsignedBytevEXT(
        static_cast<GLenum>(pname),
        data.size() ? reinterpret_cast<GLubyte*>(data.data()) : nullptr);
    detail::error_check("GetUnsignedBytevEXT"sv);
}

template<class span_u8>
requires(
    concepts::span<span_u8> &&
    std::
        is_same_v<std::decay_t<typename span_u8::value_type>, std::decay_t<u8>>)
/*!
 * \brief Part of GL_EXT_semaphore
 * \param target GLenum
 * \param index GLuint
 * \param data GLubyte *
 * \return void
 */
STATICINLINE void get_unsigned_bytei_v(GLenum target, u32 index, span_u8 data)
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
    detail::error_check("GetUnsignedBytei_vEXT"sv);
}

/*!
 * \brief Part of GL_EXT_semaphore
 * \param semaphore GLuint
 * \return Boolean
 */
STATICINLINE bool is_semaphore(u32 semaphore)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsSemaphoreEXT)
    }
    auto out = glIsSemaphoreEXT(semaphore);
    detail::error_check("IsSemaphoreEXT"sv);
    return out == GL_TRUE ? true : false;
}

template<class span_const_u64>
requires(
    concepts::span<span_const_u64> &&
    std::is_same_v<
        std::decay_t<typename span_const_u64::value_type>,
        std::decay_t<u64>>)
/*!
 * \brief Part of GL_EXT_semaphore
 * \param semaphore GLuint
 * \param pname GLenum
 * \param params const GLuint64 *
 * \return void
 */
STATICINLINE void semaphore_parameterui64v(
    u32                             semaphore,
    group::semaphore_parameter_name pname,
    span_const_u64 const&           params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SemaphoreParameterui64vEXT)
    }
    glSemaphoreParameterui64vEXT(
        semaphore,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLuint64*>(params.data())
                      : nullptr);
    detail::error_check("SemaphoreParameterui64vEXT"sv);
}

template<class span_const_texture_layout, class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>> &&
    concepts::span<span_const_texture_layout> &&
    std::is_same_v<
        std::decay_t<typename span_const_texture_layout::value_type>,
        std::decay_t<group::texture_layout>>)
/*!
 * \brief Part of GL_EXT_semaphore
 * \param semaphore GLuint
 * \param numBufferBarriers GLuint
 * \param buffers const GLuint *
 * \param numTextureBarriers GLuint
 * \param textures const GLuint *
 * \param dstLayouts const GLenum *
 * \return void
 */
STATICINLINE void signal_semaphore(
    u32                              semaphore,
    u32                              numBufferBarriers,
    span_const_u32 const&            buffers,
    u32                              numTextureBarriers,
    span_const_u32 const&            textures,
    span_const_texture_layout const& dstLayouts)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SignalSemaphoreEXT)
    }
    glSignalSemaphoreEXT(
        semaphore,
        numBufferBarriers,
        buffers.size() ? reinterpret_cast<const GLuint*>(buffers.data())
                       : nullptr,
        numTextureBarriers,
        textures.size() ? reinterpret_cast<const GLuint*>(textures.data())
                        : nullptr,
        dstLayouts.size() ? reinterpret_cast<const GLenum*>(dstLayouts.data())
                          : nullptr);
    detail::error_check("SignalSemaphoreEXT"sv);
}

template<class span_const_texture_layout, class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>> &&
    concepts::span<span_const_texture_layout> &&
    std::is_same_v<
        std::decay_t<typename span_const_texture_layout::value_type>,
        std::decay_t<group::texture_layout>>)
/*!
 * \brief Part of GL_EXT_semaphore
 * \param semaphore GLuint
 * \param numBufferBarriers GLuint
 * \param buffers const GLuint *
 * \param numTextureBarriers GLuint
 * \param textures const GLuint *
 * \param srcLayouts const GLenum *
 * \return void
 */
STATICINLINE void wait_semaphore(
    u32                              semaphore,
    u32                              numBufferBarriers,
    span_const_u32 const&            buffers,
    u32                              numTextureBarriers,
    span_const_u32 const&            textures,
    span_const_texture_layout const& srcLayouts)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WaitSemaphoreEXT)
    }
    glWaitSemaphoreEXT(
        semaphore,
        numBufferBarriers,
        buffers.size() ? reinterpret_cast<const GLuint*>(buffers.data())
                       : nullptr,
        numTextureBarriers,
        textures.size() ? reinterpret_cast<const GLuint*>(textures.data())
                        : nullptr,
        srcLayouts.size() ? reinterpret_cast<const GLenum*>(srcLayouts.data())
                          : nullptr);
    detail::error_check("WaitSemaphoreEXT"sv);
}

} // namespace gl::ext::semaphore
#endif // GL_EXT_semaphore
namespace gl::ext::semaphore {
constexpr auto name = "GL_EXT_semaphore";
}
