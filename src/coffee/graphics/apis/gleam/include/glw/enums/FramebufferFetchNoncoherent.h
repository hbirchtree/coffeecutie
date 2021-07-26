#pragma once

#include "common.h"

namespace gl::group {

// FramebufferFetchNoncoherent
enum class framebuffer_fetch_noncoherent : ::libc_types::u32
{
#ifdef GL_FRAMEBUFFER_FETCH_NONCOHERENT_QCOM
    qcom = GL_FRAMEBUFFER_FETCH_NONCOHERENT_QCOM,
#endif
}; // enum class framebuffer_fetch_noncoherent

} // namespace gl::group
