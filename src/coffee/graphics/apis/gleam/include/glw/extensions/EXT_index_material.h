#pragma once

#ifdef GL_EXT_index_material
namespace gl::ext::index_material {
namespace values {
constexpr libc_types::u32 index_material           = 0x81B8;
constexpr libc_types::u32 index_material_parameter = 0x81B9;
constexpr libc_types::u32 index_material_face      = 0x81BA;
} // namespace values
STATICINLINE void index_material(
    group::material_face face, group::index_material_parameter_ext mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IndexMaterialEXT)
    }
    glIndexMaterialEXT(static_cast<GLenum>(face), static_cast<GLenum>(mode));
    detail::error_check("IndexMaterialEXT"sv);
}

} // namespace gl::ext::index_material
#endif // GL_EXT_index_material
namespace gl::ext::index_material {
constexpr auto name = "GL_EXT_index_material";
}
