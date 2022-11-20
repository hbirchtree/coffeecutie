#pragma once

#ifdef GL_ARB_gl_spirv
namespace gl::arb::gl_spirv {
namespace values {
constexpr libc_types::u32 shader_binary_format_spir_v = 0x9551;
constexpr libc_types::u32 spir_v_binary               = 0x9552;
} // namespace values
template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_ARB_gl_spirv
     * \param shader GLuint
     * \param pEntryPoint const GLchar *
     * \param numSpecializationConstants GLuint
     * \param pConstantIndex const GLuint *
     * \param pConstantValue const GLuint *
     * \return void
     */
    STATICINLINE void specialize_shader(
        u32                     shader,
        std::string_view const& pEntryPoint,
        u32                     numSpecializationConstants,
        span_const_u32 const&   pConstantIndex,
        span_const_u32 const&   pConstantValue)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SpecializeShaderARB)
        glIsShader(shader);
    }
    glSpecializeShaderARB(
        shader,
        pEntryPoint.data(),
        numSpecializationConstants,
        pConstantIndex.size()
            ? reinterpret_cast<const GLuint*>(pConstantIndex.data())
            : nullptr,
        pConstantValue.size()
            ? reinterpret_cast<const GLuint*>(pConstantValue.data())
            : nullptr);
    detail::error_check("SpecializeShaderARB"sv);
}

} // namespace gl::arb::gl_spirv
#endif // GL_ARB_gl_spirv
namespace gl::arb::gl_spirv {
constexpr auto name = "GL_ARB_gl_spirv";
}
