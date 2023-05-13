#pragma once

#ifdef GL_ARB_draw_indirect
#include "../enums/BufferStorageTarget.h"
#include "../enums/BufferTargetARB.h"
#include "../enums/CopyBufferSubDataTarget.h"
namespace gl::arb::draw_indirect {
using gl::group::buffer_storage_target;
using gl::group::buffer_target_arb;
using gl::group::copy_buffer_sub_data_target;
namespace values {
constexpr u32 draw_indirect_buffer_binding = 0x8F43;
} // namespace values
/*!
 * \brief Part of GL_ARB_draw_indirect
 * \param mode GLenum
 * \param indirect const void *
 * \return void
 */
STATICINLINE void draw_arrays_indirect(
    group::primitive_type mode, intptr_t indirect)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawArraysIndirect)
    }
    glDrawArraysIndirect(
        static_cast<GLenum>(mode), reinterpret_cast<const void*>(indirect));
    detail::error_check("DrawArraysIndirect"sv);
}

/*!
 * \brief Part of GL_ARB_draw_indirect
 * \param mode GLenum
 * \param type GLenum
 * \param indirect const void *
 * \return void
 */
STATICINLINE void draw_elements_indirect(
    group::primitive_type     mode,
    group::draw_elements_type type,
    intptr_t                  indirect)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawElementsIndirect)
    }
    glDrawElementsIndirect(
        static_cast<GLenum>(mode),
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indirect));
    detail::error_check("DrawElementsIndirect"sv);
}

} // namespace gl::arb::draw_indirect
#endif // GL_ARB_draw_indirect
namespace gl::arb::draw_indirect {
constexpr auto name = "GL_ARB_draw_indirect";
}
