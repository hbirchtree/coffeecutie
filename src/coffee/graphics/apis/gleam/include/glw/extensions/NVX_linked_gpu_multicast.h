#pragma once

#ifdef GL_NVX_linked_gpu_multicast
#include "../enums/BufferStorageMask.h"
namespace gl::nvx::linked_gpu_multicast {
using gl::group::buffer_storage_mask;
namespace values {
constexpr libc_types::u32 max_lgpu_gpus = 0x92BA;
} // namespace values
template<class size_3_i32>
requires(semantic::concepts::Size2D<size_3_i32, i32>)
    /*!
     * \brief Part of GL_NVX_linked_gpu_multicast
     * \param sourceGpu GLuint
     * \param destinationGpuMask GLbitfield
     * \param srcName GLuint
     * \param srcTarget GLenum
     * \param srcLevel GLint
     * \param srcX GLint
     * \param srxY GLint
     * \param srcZ GLint
     * \param dstName GLuint
     * \param dstTarget GLenum
     * \param dstLevel GLint
     * \param dstX GLint
     * \param dstY GLint
     * \param dstZ GLint
     * \param width GLsizei
     * \param height GLsizei
     * \param depth GLsizei
     * \return void
     */
    STATICINLINE void lgpu_copy_image_sub_data(
        u32               sourceGpu,
        GLbitfield        destinationGpuMask,
        u32               srcName,
        GLenum            srcTarget,
        i32               srcLevel,
        i32               srcX,
        i32               srxY,
        i32               srcZ,
        u32               dstName,
        GLenum            dstTarget,
        i32               dstLevel,
        i32               dstX,
        i32               dstY,
        i32               dstZ,
        size_3_i32 const& width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LGPUCopyImageSubDataNVX)
    }
    glLGPUCopyImageSubDataNVX(
        sourceGpu,
        destinationGpuMask,
        srcName,
        srcTarget,
        srcLevel,
        srcX,
        srxY,
        srcZ,
        dstName,
        dstTarget,
        dstLevel,
        dstX,
        dstY,
        dstZ,
        width[0],
        width[1],
        width[2]);
    detail::error_check("LGPUCopyImageSubDataNVX"sv);
}

/*!
 * \brief Part of GL_NVX_linked_gpu_multicast

 * \return void
 */
STATICINLINE void lgpu_interlock()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LGPUInterlockNVX)
    }
    glLGPUInterlockNVX();
    detail::error_check("LGPUInterlockNVX"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Part of GL_NVX_linked_gpu_multicast
     * \param gpuMask GLbitfield
     * \param buffer GLuint
     * \param offset GLintptr
     * \param size GLsizeiptr
     * \param data const void *
     * \return void
     */
    STATICINLINE void lgpu_named_buffer_sub_data(
        GLbitfield             gpuMask,
        u32                    buffer,
        GLintptr               offset,
        GLsizeiptr             size,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LGPUNamedBufferSubDataNVX)
        glIsBuffer(buffer);
    }
    glLGPUNamedBufferSubDataNVX(
        gpuMask,
        buffer,
        offset,
        size,
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("LGPUNamedBufferSubDataNVX"sv);
}

} // namespace gl::nvx::linked_gpu_multicast
#endif // GL_NVX_linked_gpu_multicast
namespace gl::nvx::linked_gpu_multicast {
constexpr auto name = "GL_NVX_linked_gpu_multicast";
}
