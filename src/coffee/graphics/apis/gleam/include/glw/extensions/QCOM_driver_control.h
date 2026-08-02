#pragma once

#ifdef GL_QCOM_driver_control
namespace gl::qcom::driver_control {
namespace values {
} // namespace values

/*!
 * \brief Part of GL_QCOM_driver_control
 * \param driverControl GLuint
 * \return void
 */
STATICINLINE void disable_driver_control(
    u32 driverControl, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(DisableDriverControlQCOM)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glDisableDriverControlQCOM, driverControl);
#endif
    glDisableDriverControlQCOM(driverControl);
    detail::error_check("DisableDriverControlQCOM"sv, check_errors);
}

/*!
 * \brief Part of GL_QCOM_driver_control
 * \param driverControl GLuint
 * \return void
 */
STATICINLINE void enable_driver_control(
    u32 driverControl, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(EnableDriverControlQCOM)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glEnableDriverControlQCOM, driverControl);
#endif
    glEnableDriverControlQCOM(driverControl);
    detail::error_check("EnableDriverControlQCOM"sv, check_errors);
}

template<class span_GLchar, class span_i32>
requires(
    concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>> &&
    concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>>)
/*!
 * \brief Part of GL_QCOM_driver_control
 * \param driverControl GLuint
 * \param bufSize GLsizei
 * \param length GLsizei *
 * \param driverControlString GLchar *
 * \return void
 */
STATICINLINE void get_driver_control_string(
    u32         driverControl,
    span_i32    length,
    span_GLchar driverControlString,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetDriverControlStringQCOM)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glGetDriverControlStringQCOM,
        driverControl,
        gsl::span<char>(
            reinterpret_cast<char*>(length.data()), length.size_bytes()),
        gsl::span<char>(
            reinterpret_cast<char*>(driverControlString.data()),
            driverControlString.size_bytes()));
#endif
    glGetDriverControlStringQCOM(
        driverControl,
        driverControlString.size(),
        length.size() ? reinterpret_cast<GLsizei*>(length.data()) : nullptr,
        driverControlString.data());
    detail::error_check("GetDriverControlStringQCOM"sv, check_errors);
}

template<class span_i32, class span_u32>
requires(
    concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>> &&
    concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_QCOM_driver_control
 * \param num GLint *
 * \param size GLsizei
 * \param driverControls GLuint *
 * \return void
 */
STATICINLINE void get_driver_controls(
    span_i32    num,
    span_u32    driverControls,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetDriverControlsQCOM)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glGetDriverControlsQCOM,
        gsl::span<char>(reinterpret_cast<char*>(num.data()), num.size_bytes()),
        gsl::span<char>(
            reinterpret_cast<char*>(driverControls.data()),
            driverControls.size_bytes()));
#endif
    glGetDriverControlsQCOM(
        num.size() ? reinterpret_cast<GLint*>(num.data()) : nullptr,
        driverControls.size(),
        driverControls.size() ? reinterpret_cast<GLuint*>(driverControls.data())
                              : nullptr);
    detail::error_check("GetDriverControlsQCOM"sv, check_errors);
}

} // namespace gl::qcom::driver_control
#endif // GL_QCOM_driver_control
namespace gl::qcom::driver_control {
constexpr auto name = "GL_QCOM_driver_control";
}
