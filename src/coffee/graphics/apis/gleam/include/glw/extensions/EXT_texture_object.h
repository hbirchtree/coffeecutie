#pragma once

#ifdef GL_EXT_texture_object
#include "../enums/GetPName.h"
#include "../enums/TextureParameterName.h"
namespace gl::ext::texture_object {
using gl::group::get_prop;
using gl::group::texture_parameter_name;
namespace values {
constexpr libc_types::u32 texture_resident   = 0x8067;
constexpr libc_types::u32 texture_1d_binding = 0x8068;
constexpr libc_types::u32 texture_2d_binding = 0x8069;
} // namespace values
template<class span_bool, class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>&& semantic::concepts::Span<span_bool>&&
                              std::is_same_v<
                 std::decay_t<typename span_bool::value_type>,
                 std::decay_t<bool>>) STATICINLINE GLboolean
    are_textures_resident(span_const_u32 const& textures, span_bool residences)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(AreTexturesResidentEXT)
    }
    auto out = glAreTexturesResidentEXT(
        textures.size(),
        textures.size() ? reinterpret_cast<const GLuint*>(textures.data())
                        : nullptr,
        residences.size() ? reinterpret_cast<GLboolean*>(residences.data())
                          : nullptr);
    detail::error_check("AreTexturesResidentEXT"sv);
    return out;
}

STATICINLINE void bind_texture(group::texture_target target, u32 texture)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindTextureEXT)
        glIsTexture(texture);
    }
    glBindTextureEXT(static_cast<GLenum>(target), texture);
    detail::error_check("BindTextureEXT"sv);
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void delete_textures(span_const_u32 const& textures)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteTexturesEXT)
    }
    glDeleteTexturesEXT(
        textures.size(),
        textures.size() ? reinterpret_cast<const GLuint*>(textures.data())
                        : nullptr);
    detail::error_check("DeleteTexturesEXT"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE void gen_textures(span_u32 textures)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenTexturesEXT)
    }
    glGenTexturesEXT(
        textures.size(),
        textures.size() ? reinterpret_cast<GLuint*>(textures.data()) : nullptr);
    detail::error_check("GenTexturesEXT"sv);
}

STATICINLINE GLboolean is_texture(u32 texture)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsTextureEXT)
        glIsTexture(texture);
    }
    auto out = glIsTextureEXT(texture);
    detail::error_check("IsTextureEXT"sv);
    return out;
}

template<class span_const_GLclampf, class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>&& semantic::concepts::Span<span_const_GLclampf>&&
                              std::is_same_v<
                 std::decay_t<typename span_const_GLclampf::value_type>,
                 std::decay_t<GLclampf>>) STATICINLINE
    void prioritize_textures(
        span_const_u32 const& textures, span_const_GLclampf const& priorities)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PrioritizeTexturesEXT)
    }
    glPrioritizeTexturesEXT(
        textures.size(),
        textures.size() ? reinterpret_cast<const GLuint*>(textures.data())
                        : nullptr,
        priorities.size() ? reinterpret_cast<const GLclampf*>(priorities.data())
                          : nullptr);
    detail::error_check("PrioritizeTexturesEXT"sv);
}

} // namespace gl::ext::texture_object
#endif // GL_EXT_texture_object
namespace gl::ext::texture_object {
constexpr auto name = "GL_EXT_texture_object";
}
