#pragma once

#ifdef GL_QCOM_writeonly_rendering
namespace gl::qcom::writeonly_rendering {
namespace values {
constexpr libc_types::u32 writeonly_rendering = 0x8823;
} // namespace values
} // namespace gl::qcom::writeonly_rendering
#endif // GL_QCOM_writeonly_rendering
namespace gl::qcom::writeonly_rendering {
constexpr auto name = "GL_QCOM_writeonly_rendering";
}
