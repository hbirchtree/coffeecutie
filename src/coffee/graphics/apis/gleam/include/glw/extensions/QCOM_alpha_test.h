#pragma once

#ifdef GL_QCOM_alpha_test
#include "../enums/GetPName.h"
namespace gl::qcom::alpha_test {
using gl::group::get_prop;
namespace values {
} // namespace values
STATICINLINE void alpha_func(GLenum func, GLclampf ref)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(AlphaFuncQCOM)
    }
    glAlphaFuncQCOM(func, ref);
    detail::error_check("AlphaFuncQCOM"sv);
}

} // namespace gl::qcom::alpha_test
#endif // GL_QCOM_alpha_test
namespace gl::qcom::alpha_test {
constexpr auto name = "GL_QCOM_alpha_test";
}
