#pragma once

#ifdef GL_EXT_clip_cull_distance
namespace gl::ext::clip_cull_distance {
namespace values {
constexpr libc_types::u32 max_clip_distances                   = 0x0D32;
constexpr libc_types::u32 clip_distance0                       = 0x3000;
constexpr libc_types::u32 clip_distance1                       = 0x3001;
constexpr libc_types::u32 clip_distance2                       = 0x3002;
constexpr libc_types::u32 clip_distance3                       = 0x3003;
constexpr libc_types::u32 clip_distance4                       = 0x3004;
constexpr libc_types::u32 clip_distance5                       = 0x3005;
constexpr libc_types::u32 clip_distance6                       = 0x3006;
constexpr libc_types::u32 clip_distance7                       = 0x3007;
constexpr libc_types::u32 max_cull_distances                   = 0x82F9;
constexpr libc_types::u32 max_combined_clip_and_cull_distances = 0x82FA;
} // namespace values
} // namespace gl::ext::clip_cull_distance
#endif // GL_EXT_clip_cull_distance
namespace gl::ext::clip_cull_distance {
constexpr auto name = "GL_EXT_clip_cull_distance";
}
