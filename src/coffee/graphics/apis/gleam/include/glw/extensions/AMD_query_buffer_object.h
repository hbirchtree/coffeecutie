#pragma once

#ifdef GL_AMD_query_buffer_object
namespace gl::amd::query_buffer_object {
namespace values {
constexpr u32 query_buffer         = 0x9192;
constexpr u32 query_buffer_binding = 0x9193;
constexpr u32 query_result_no_wait = 0x9194;
} // namespace values
} // namespace gl::amd::query_buffer_object
#endif // GL_AMD_query_buffer_object
namespace gl::amd::query_buffer_object {
constexpr auto name = "GL_AMD_query_buffer_object";
}
