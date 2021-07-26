#pragma once

#include "common.h"

namespace gl::group {

// ExternalHandleType
enum class external_handle_type : ::libc_types::u32
{
#ifdef GL_HANDLE_TYPE_D3D11_IMAGE_EXT
    handle_type_d3d11_image_ext = GL_HANDLE_TYPE_D3D11_IMAGE_EXT,
#endif
#ifdef GL_HANDLE_TYPE_D3D11_IMAGE_KMT_EXT
    handle_type_d3d11_image_kmt_ext = GL_HANDLE_TYPE_D3D11_IMAGE_KMT_EXT,
#endif
#ifdef GL_HANDLE_TYPE_D3D12_FENCE_EXT
    handle_type_d3d12_fence_ext = GL_HANDLE_TYPE_D3D12_FENCE_EXT,
#endif
#ifdef GL_HANDLE_TYPE_D3D12_RESOURCE_EXT
    handle_type_d3d12_resource_ext = GL_HANDLE_TYPE_D3D12_RESOURCE_EXT,
#endif
#ifdef GL_HANDLE_TYPE_D3D12_TILEPOOL_EXT
    handle_type_d3d12_tilepool_ext = GL_HANDLE_TYPE_D3D12_TILEPOOL_EXT,
#endif
#ifdef GL_HANDLE_TYPE_OPAQUE_FD_EXT
    handle_type_opaque_fd_ext = GL_HANDLE_TYPE_OPAQUE_FD_EXT,
#endif
#ifdef GL_HANDLE_TYPE_OPAQUE_WIN32_EXT
    handle_type_opaque_win32_ext = GL_HANDLE_TYPE_OPAQUE_WIN32_EXT,
#endif
#ifdef GL_HANDLE_TYPE_OPAQUE_WIN32_KMT_EXT
    handle_type_opaque_win32_kmt_ext = GL_HANDLE_TYPE_OPAQUE_WIN32_KMT_EXT,
#endif
}; // enum class external_handle_type

} // namespace gl::group
