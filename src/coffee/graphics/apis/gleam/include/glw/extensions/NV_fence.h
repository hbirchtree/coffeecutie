#pragma once

#ifdef GL_NV_fence
#include "../enums/FenceConditionNV.h"
#include "../enums/FenceParameterNameNV.h"

namespace gl::nv::fence {
using gl::group::fence_condition_nv;
using gl::group::fence_parameter_name_nv;

namespace values {
} // namespace values

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_NV_fence
 * \param n GLsizei
 * \param fences const GLuint *
 * \return void
 */
STATICINLINE void delete_fences(
    span_const_u32 const& fences, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteFencesNV)
    }
    glDeleteFencesNV(
        fences.size(),
        fences.size() ? reinterpret_cast<const GLuint*>(fences.data())
                      : nullptr);
    detail::error_check("DeleteFencesNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_fence
 * \param fence GLuint
 * \return void
 */
STATICINLINE void finish_fence(
    u32 fence, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FinishFenceNV)
    }
    glFinishFenceNV(fence);
    detail::error_check("FinishFenceNV"sv, check_errors);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_NV_fence
 * \param n GLsizei
 * \param fences GLuint *
 * \return void
 */
STATICINLINE void gen_fences(
    span_u32 fences, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenFencesNV)
    }
    glGenFencesNV(
        fences.size(),
        fences.size() ? reinterpret_cast<GLuint*>(fences.data()) : nullptr);
    detail::error_check("GenFencesNV"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_NV_fence
 * \param fence GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_fenceiv(
    u32                            fence,
    group::fence_parameter_name_nv pname,
    span_i32                       params,
    error_check                    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFenceivNV)
    }
    glGetFenceivNV(
        fence,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetFenceivNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_fence
 * \param fence GLuint
 * \return Boolean
 */
STATICINLINE bool is_fence(
    u32 fence, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsFenceNV)
    }
    auto out = glIsFenceNV(fence);
    detail::error_check("IsFenceNV"sv, check_errors);
    return out == GL_TRUE ? true : false;
}

/*!
 * \brief Part of GL_NV_fence
 * \param fence GLuint
 * \param condition GLenum
 * \return void
 */
STATICINLINE void set_fence(
    u32                       fence,
    group::fence_condition_nv condition,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SetFenceNV)
    }
    glSetFenceNV(fence, static_cast<GLenum>(condition));
    detail::error_check("SetFenceNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_fence
 * \param fence GLuint
 * \return Boolean
 */
STATICINLINE bool test_fence(
    u32 fence, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TestFenceNV)
    }
    auto out = glTestFenceNV(fence);
    detail::error_check("TestFenceNV"sv, check_errors);
    return out == GL_TRUE ? true : false;
}

} // namespace gl::nv::fence
#endif // GL_NV_fence
namespace gl::nv::fence {
constexpr auto name = "GL_NV_fence";
}
