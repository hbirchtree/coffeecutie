#pragma once

#include "types/tdef/integertypes.h"
#include "coffee_mem_macros.h"
#include "plat/plat_primary_identify.h"

namespace Coffee{
struct PlatformData
{
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
