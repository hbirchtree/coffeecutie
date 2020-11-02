#pragma once

#include "levels/all_levels.h"

namespace gl {

#if GL_VERSION_VERIFY(0x100, GL_VERSION_NONE)
constexpr bool is_core = true;
constexpr bool is_es   = false;
#else
constexpr bool is_core = false;
constexpr bool is_es      = true;
#endif

#if GL_VERSION_VERIFY(0x100, GL_VERSION_NONE)
using v33 = glwrap::v33<GLVER_33>;
using v40 = glwrap::v40<GLVER_40>;
using v41 = glwrap::v41<GLVER_41>;
using v42 = glwrap::v42<GLVER_42>;
using v43 = glwrap::v43<GLVER_43>;
using v44 = glwrap::v44<GLVER_44>;
using v45 = glwrap::v45<GLVER_45>;
using v46 = glwrap::v46<GLVER_46>;
#else
using v33 = glwrap::v30es<GLESVER_30>;
using v41 = glwrap::v31es<GLESVER_31>;
using v43 = glwrap::v32es<GLESVER_32>;
#endif

using v20es = glwrap::v20es<GLESVER_20>;
using v30es = glwrap::v30es<GLESVER_30>;
using v31es = glwrap::v31es<GLESVER_31>;
using v32es = glwrap::v32es<GLESVER_32>;

#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
using vlow = v33;
#elif GL_VERSION_VERIFY(GL_VERSION_NONE, 0x200)
using vlow                = v30es;
#endif

struct common
{
#if GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)
    using khr_debug = gl::v43;
#elif GL_VERSION_VERIFY(GL_VERSION_NONE, 0x320)
    using khr_debug = gl::v32es;
#endif

    STATICINLINE void PushDebugGroup(libc_types::cstring name)
    {
#if GL_VERSION_VERIFY(0x430, 0x320)
        khr_debug::PushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, {}, -1, name);
#endif
    }
    STATICINLINE void PopDebugGroup()
    {
#if GL_VERSION_VERIFY(0x430, 0x320)
        khr_debug::PopDebugGroup();
#endif
    }
    STATICINLINE void DebugMessageInsert(
        semantic::debug::Severity severity,
        semantic::debug::Type     type,
        stl_types::CString const& message)
    {
#if GL_VERSION_VERIFY(0x430, 0x320)
        khr_debug::DebugMessageInsert(
            GL_DEBUG_SOURCE_APPLICATION,
            Coffee::CGL::to_enum(severity),
            {},
            Coffee::CGL::to_enum(type),
            -1,
            message.c_str());
#endif
    }
};

} // namespace gl
