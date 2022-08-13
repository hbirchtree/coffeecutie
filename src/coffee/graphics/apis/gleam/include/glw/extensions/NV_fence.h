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
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void delete_fences(span_const_u32 const& fences)
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
    detail::error_check("DeleteFencesNV"sv);
}

STATICINLINE void finish_fence(u32 fence)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FinishFenceNV)
    }
    glFinishFenceNV(fence);
    detail::error_check("FinishFenceNV"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE void gen_fences(span_u32 fences)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenFencesNV)
    }
    glGenFencesNV(
        fences.size(),
        fences.size() ? reinterpret_cast<GLuint*>(fences.data()) : nullptr);
    detail::error_check("GenFencesNV"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_fenceiv(
        u32 fence, group::fence_parameter_name_nv pname, span_i32 params)
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
    detail::error_check("GetFenceivNV"sv);
}

STATICINLINE GLboolean is_fence(u32 fence)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsFenceNV)
    }
    auto out = glIsFenceNV(fence);
    detail::error_check("IsFenceNV"sv);
    return out;
}

STATICINLINE void set_fence(u32 fence, group::fence_condition_nv condition)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SetFenceNV)
    }
    glSetFenceNV(fence, static_cast<GLenum>(condition));
    detail::error_check("SetFenceNV"sv);
}

STATICINLINE GLboolean test_fence(u32 fence)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TestFenceNV)
    }
    auto out = glTestFenceNV(fence);
    detail::error_check("TestFenceNV"sv);
    return out;
}

} // namespace gl::nv::fence
#endif // GL_NV_fence
namespace gl::nv::fence {
constexpr auto name = "GL_NV_fence";
}