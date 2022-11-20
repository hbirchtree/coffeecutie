#pragma once

#ifdef GL_ARB_shader_atomic_counters
#include "../enums/AtomicCounterBufferPName.h"
#include "../enums/BufferStorageTarget.h"
#include "../enums/BufferTargetARB.h"
#include "../enums/CopyBufferSubDataTarget.h"
#include "../enums/GetPName.h"
#include "../enums/ProgramPropertyARB.h"
#include "../enums/UniformPName.h"
namespace gl::arb::shader_atomic_counters {
using gl::group::atomic_counter_buffer_prop;
using gl::group::buffer_storage_target;
using gl::group::buffer_target_arb;
using gl::group::copy_buffer_sub_data_target;
using gl::group::get_prop;
using gl::group::program_property_arb;
using gl::group::uniform_prop;
namespace values {
constexpr libc_types::u32 atomic_counter_buffer_start                = 0x92C2;
constexpr libc_types::u32 atomic_counter_buffer_size                 = 0x92C3;
constexpr libc_types::u32 max_vertex_atomic_counter_buffers          = 0x92CC;
constexpr libc_types::u32 max_tess_control_atomic_counter_buffers    = 0x92CD;
constexpr libc_types::u32 max_tess_evaluation_atomic_counter_buffers = 0x92CE;
constexpr libc_types::u32 max_geometry_atomic_counter_buffers        = 0x92CF;
constexpr libc_types::u32 max_fragment_atomic_counter_buffers        = 0x92D0;
constexpr libc_types::u32 max_combined_atomic_counter_buffers        = 0x92D1;
constexpr libc_types::u32 max_atomic_counter_buffer_size             = 0x92D8;
constexpr libc_types::u32 unsigned_int_atomic_counter                = 0x92DB;
constexpr libc_types::u32 max_atomic_counter_buffer_bindings         = 0x92DC;
} // namespace values
template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_ARB_shader_atomic_counters
     * \param program GLuint
     * \param bufferIndex GLuint
     * \param pname GLenum
     * \param params GLint *
     * \return void
     */
    STATICINLINE void get_active_atomic_counter_bufferiv(
        u32                               program,
        u32                               bufferIndex,
        group::atomic_counter_buffer_prop pname,
        span_i32                          params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveAtomicCounterBufferiv)
        glIsProgram(program);
    }
    glGetActiveAtomicCounterBufferiv(
        program,
        bufferIndex,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetActiveAtomicCounterBufferiv"sv);
}

} // namespace gl::arb::shader_atomic_counters
#endif // GL_ARB_shader_atomic_counters
namespace gl::arb::shader_atomic_counters {
constexpr auto name = "GL_ARB_shader_atomic_counters";
}
