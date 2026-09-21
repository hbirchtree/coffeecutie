#pragma once

#include "common.h"

namespace gl::group {

// ExternalHandleType
enum class external_handle_type : u32
{
    handle_type_d3d11_image_ext = 0x958B, // GL_HANDLE_TYPE_D3D11_IMAGE_EXT
    handle_type_d3d11_image_kmt_ext =
        0x958C,                           // GL_HANDLE_TYPE_D3D11_IMAGE_KMT_EXT
    handle_type_d3d12_fence_ext = 0x9594, // GL_HANDLE_TYPE_D3D12_FENCE_EXT
    handle_type_d3d12_resource_ext =
        0x958A, // GL_HANDLE_TYPE_D3D12_RESOURCE_EXT
    handle_type_d3d12_tilepool_ext =
        0x9589,                            // GL_HANDLE_TYPE_D3D12_TILEPOOL_EXT
    handle_type_opaque_fd_ext    = 0x9586, // GL_HANDLE_TYPE_OPAQUE_FD_EXT
    handle_type_opaque_win32_ext = 0x9587, // GL_HANDLE_TYPE_OPAQUE_WIN32_EXT
    handle_type_opaque_win32_kmt_ext =
        0x9588, // GL_HANDLE_TYPE_OPAQUE_WIN32_KMT_EXT
}; // enum class external_handle_type

} // namespace gl::group
