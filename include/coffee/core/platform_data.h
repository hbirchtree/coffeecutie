#pragma once

#include "types/tdef/stltypes.h"

namespace Coffee{
struct PlatformData
{
    static
    /*!
     * \brief Displayable string of system and its version
     */
    CString SystemDisplayString();

    static
    /*!
     * \brief Defined on mobile devices, phones and tablets mostly
     * \return
     */
    bool IsMobile();

    static
    /*!
     * \brief Defined when OpenGL ES is the graphics API
     * \return
     */
    bool IsGLES();

    static
    /*!
     * \brief Defined for systems where some storage is not file-based, such as Android assets
     * \return
     */
    bool UseVirtualFS();

    static
    /*!
     * \brief IsDebug
     * \return
     */
    bool IsDebug();
};
}
