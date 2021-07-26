#pragma once

#ifdef GL_QCOM_driver_control
namespace gl::qcom::driver_control {
namespace values {
} // namespace values
STATICINLINE void disable_driver_control(u32 driverControl)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DisableDriverControlQCOM)
    }
    glDisableDriverControlQCOM(driverControl);
    detail::error_check("DisableDriverControlQCOM"sv);
}

STATICINLINE void enable_driver_control(u32 driverControl)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EnableDriverControlQCOM)
    }
    glEnableDriverControlQCOM(driverControl);
    detail::error_check("EnableDriverControlQCOM"sv);
}

template<class span_GLchar, class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>&& semantic::concepts::Span<span_GLchar>&&
                              std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>) STATICINLINE
    void get_driver_control_string(
        u32 driverControl, span_i32 length, span_GLchar driverControlString)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetDriverControlStringQCOM)
    }
    glGetDriverControlStringQCOM(
        driverControl,
        driverControlString.size(),
        length.size() ? reinterpret_cast<GLsizei*>(length.data()) : nullptr,
        driverControlString.data());
    detail::error_check("GetDriverControlStringQCOM"sv);
}

template<class span_i32, class span_u32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>&& semantic::concepts::Span<span_u32>&&
                              std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void get_driver_controls(span_i32 num, span_u32 driverControls)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetDriverControlsQCOM)
    }
    glGetDriverControlsQCOM(
        num.size() ? reinterpret_cast<GLint*>(num.data()) : nullptr,
        driverControls.size(),
        driverControls.size() ? reinterpret_cast<GLuint*>(driverControls.data())
                              : nullptr);
    detail::error_check("GetDriverControlsQCOM"sv);
}

} // namespace gl::qcom::driver_control
#endif // GL_QCOM_driver_control
namespace gl::qcom::driver_control {
constexpr auto name = "GL_QCOM_driver_control";
}
