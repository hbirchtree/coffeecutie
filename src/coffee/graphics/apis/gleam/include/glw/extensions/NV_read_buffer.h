#pragma once

#ifdef GL_NV_read_buffer
#include "../enums/GetPName.h"

namespace gl::nv::read_buffer {
using gl::group::get_prop;

namespace values {
} // namespace values

/*!
 * \brief Part of GL_NV_read_buffer
 * \param mode GLenum
 * \return void
 */
STATICINLINE void read_buffer(GLenum mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ReadBufferNV)
    }
    glReadBufferNV(mode);
    detail::error_check("ReadBufferNV"sv);
}

} // namespace gl::nv::read_buffer
#endif // GL_NV_read_buffer
namespace gl::nv::read_buffer {
constexpr auto name = "GL_NV_read_buffer";
}
