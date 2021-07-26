#pragma once

#ifdef GL_QCOM_perfmon_global_mode
namespace gl::qcom::perfmon_global_mode {
namespace values {
constexpr libc_types::u32 perfmon_global_mode = 0x8FA0;
} // namespace values
} // namespace gl::qcom::perfmon_global_mode
#endif // GL_QCOM_perfmon_global_mode
namespace gl::qcom::perfmon_global_mode {
constexpr auto name = "GL_QCOM_perfmon_global_mode";
}
