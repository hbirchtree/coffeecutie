#pragma once

#ifdef GL_ARB_transform_feedback_overflow_query
namespace gl::arb::transform_feedback_overflow_query {
namespace values {
constexpr libc_types::u32 transform_feedback_overflow        = 0x82EC;
constexpr libc_types::u32 transform_feedback_stream_overflow = 0x82ED;
} // namespace values
} // namespace gl::arb::transform_feedback_overflow_query
#endif // GL_ARB_transform_feedback_overflow_query
namespace gl::arb::transform_feedback_overflow_query {
constexpr auto name = "GL_ARB_transform_feedback_overflow_query";
}
