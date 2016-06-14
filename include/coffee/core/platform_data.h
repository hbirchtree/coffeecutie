#pragma once

#include "types/tdef/integertypes.h"
#include "types/tdef/stltypes.h"
#include "coffee_mem_macros.h"
#include "plat/plat_primary_identify.h"
#include "plat/plat_arch_identify.h"
#include "plat/plat_environment.h"

namespace Coffee{
struct PlatformData
{
    STATICINLINE
    /*!
     * \brief Displayable string of system and its version
     */
    CString SystemDisplayString()
    {
        const constexpr cstring _fmt = "%s %s %u-bit (%s ";
        CString sys_ver = SysInfo::GetSystemVersion();
        int len = snprintf(nullptr,0,_fmt,
                           C_SYSTEM_STRING,
                           sys_ver.c_str(),
                           C_SYSTEM_BITNESS,
                           COFFEE_ARCH);
        CString base;
        base.resize(len);
        snprintf(&base[0],base.size(),_fmt,
                 C_SYSTEM_STRING,
                 sys_ver.c_str(),
                 C_SYSTEM_BITNESS,
                 COFFEE_ARCH);
        base.resize(base.find('\0'));
        base.append(")");
        return base;
    }

    STATICINLINE
    /*!
     * \brief Defined on mobile devices, phones and tablets mostly
     * \return
     */
    bool IsMobile()
    {
#if !defined(COFFEE_ANDROID) && !defined(COFFEE_IOS)
        return false;
#else
        return true;
#endif
    }

    STATICINLINE
    /*!
     * \brief Defined when OpenGL ES is the graphics API
     * \return
     */
    bool IsGLES()
    {
#ifdef COFFEE_GLEAM_DESKTOP
        return false;
#else
        return true;
#endif
    }

    STATICINLINE
    /*!
     * \brief Defined for systems where some storage is not file-based, such as Android assets
     * \return
     */
    bool UseVirtualFS()
    {
#if defined(COFFEE_ANDROID)
        return true;
#else
        return false;
#endif
    }
};
}
