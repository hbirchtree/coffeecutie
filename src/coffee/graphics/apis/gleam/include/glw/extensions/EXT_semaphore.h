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
STATICINLINE void delete_semaphores(
    span_const_u32 const& semaphores,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(DeleteSemaphoresEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glDeleteSemaphoresEXT,
        gsl::span<const char>(
            reinterpret_cast<const char*>(semaphores.data()),
            semaphores.size_bytes()));
#endif
    glDeleteSemaphoresEXT(
        semaphores.size(),
        semaphores.size() ? reinterpret_cast<const GLuint*>(semaphores.data())
                          : nullptr);
    detail::error_check("DeleteSemaphoresEXT"sv, check_errors);
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
STATICINLINE void gen_semaphores(
    span_u32 semaphores, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GenSemaphoresEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGenSemaphoresEXT,
        gsl::span<char>(
            reinterpret_cast<char*>(semaphores.data()),
            semaphores.size_bytes()));
#endif
    glGenSemaphoresEXT(
        semaphores.size(),
        semaphores.size() ? reinterpret_cast<GLuint*>(semaphores.data())
                          : nullptr);
    detail::error_check("GenSemaphoresEXT"sv, check_errors);
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
    u32                             semaphore,
    group::semaphore_parameter_name pname,
    span_u64                        params,
    error_check                     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetSemaphoreParameterui64vEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetSemaphoreParameterui64vEXT,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        semaphore,
        pname);
#endif
    glGetSemaphoreParameterui64vEXT(
        semaphore,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint64*>(params.data()) : nullptr);
    detail::error_check("GetSemaphoreParameterui64vEXT"sv, check_errors);
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
STATICINLINE void get_unsigned_bytev(
    group::get_prop pname,
    span_u8         data,
    error_check     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetUnsignedBytevEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetUnsignedBytevEXT,
        gsl::span<char>(
            reinterpret_cast<char*>(data.data()), data.size_bytes()),
        pname);
#endif
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
 * \brief Part of GL_EXT_semaphore
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
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetUnsignedBytei_vEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetUnsignedBytei_vEXT,
        gsl::span<char>(
            reinterpret_cast<char*>(data.data()), data.size_bytes()),
        target,
        index);
#endif
    glGetUnsignedBytei_vEXT(
        target,
        index,
        data.size() ? reinterpret_cast<GLubyte*>(data.data()) : nullptr);
    detail::error_check("GetUnsignedBytei_vEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_semaphore
 * \param semaphore GLuint
 * \return GLboolean
 */
STATICINLINE GLboolean
is_semaphore(u32 semaphore, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(IsSemaphoreEXT)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glIsSemaphoreEXT, semaphore);
#endif
    auto out = glIsSemaphoreEXT(semaphore);
    detail::error_check("IsSemaphoreEXT"sv, check_errors);
    return out;
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
    span_const_u64 const&           params,
    error_check                     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(SemaphoreParameterui64vEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glSemaphoreParameterui64vEXT,
        gsl::span<const char>(
            reinterpret_cast<const char*>(params.data()), params.size_bytes()),
        semaphore,
        pname);
#endif
    glSemaphoreParameterui64vEXT(
        semaphore,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLuint64*>(params.data())
                      : nullptr);
    detail::error_check("SemaphoreParameterui64vEXT"sv, check_errors);
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
    span_const_texture_layout const& dstLayouts,
    error_check                      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(SignalSemaphoreEXT)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glSignalSemaphoreEXT,
        semaphore,
        numBufferBarriers,
        gsl::span<const char>(
            reinterpret_cast<const char*>(buffers.data()),
            buffers.size_bytes()),
        numTextureBarriers,
        gsl::span<const char>(
            reinterpret_cast<const char*>(textures.data()),
            textures.size_bytes()),
        gsl::span<const char>(
            reinterpret_cast<const char*>(dstLayouts.data()),
            dstLayouts.size_bytes()));
#endif
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
    detail::error_check("SignalSemaphoreEXT"sv, check_errors);
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
    span_const_texture_layout const& srcLayouts,
    error_check                      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(WaitSemaphoreEXT)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glWaitSemaphoreEXT,
        semaphore,
        numBufferBarriers,
        gsl::span<const char>(
            reinterpret_cast<const char*>(buffers.data()),
            buffers.size_bytes()),
        numTextureBarriers,
        gsl::span<const char>(
            reinterpret_cast<const char*>(textures.data()),
            textures.size_bytes()),
        gsl::span<const char>(
            reinterpret_cast<const char*>(srcLayouts.data()),
            srcLayouts.size_bytes()));
#endif
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
    detail::error_check("WaitSemaphoreEXT"sv, check_errors);
}

} // namespace gl::ext::semaphore
#endif // GL_EXT_semaphore
namespace gl::ext::semaphore {
constexpr auto name = "GL_EXT_semaphore";
}
