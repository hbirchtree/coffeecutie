#pragma once

#ifdef GL_EXT_index_array_formats
namespace gl::ext::index_array_formats {
namespace values {
constexpr libc_types::u32 iui_v2f         = 0x81AD;
constexpr libc_types::u32 iui_v3f         = 0x81AE;
constexpr libc_types::u32 iui_n3f_v2f     = 0x81AF;
constexpr libc_types::u32 iui_n3f_v3f     = 0x81B0;
constexpr libc_types::u32 t2f_iui_v2f     = 0x81B1;
constexpr libc_types::u32 t2f_iui_v3f     = 0x81B2;
constexpr libc_types::u32 t2f_iui_n3f_v2f = 0x81B3;
constexpr libc_types::u32 t2f_iui_n3f_v3f = 0x81B4;
} // namespace values
} // namespace gl::ext::index_array_formats
#endif // GL_EXT_index_array_formats
namespace gl::ext::index_array_formats {
constexpr auto name = "GL_EXT_index_array_formats";
}
