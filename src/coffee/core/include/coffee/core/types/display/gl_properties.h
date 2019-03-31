#pragma once

#include "context_bits.h"
#include "gl_version.h"
#include <peripherals/enum/helpers.h>

namespace Coffee {
namespace Display {
namespace GL {

/*!
 * \brief GL context properties to set on start-up
 */
struct Properties
{
    Properties() : bits(), flags(GLCoreProfile), version(3, 3)
    {
    }

    enum Flags : uint32
    {
        GLNoFlag          = 0x00,
        GLCoreProfile     = 0x01,  /*!< Set GL core profile*/
        GLVSync           = 0x02,  /*!< Set GL vertical sync*/
        GLDebug           = 0x04,  /*!< Set GL debug context*/
        GLAutoResize      = 0x08,  /*!< Set GL auto resize of context*/
        GLRobust          = 0x10,  /*!< Set GL robustness*/
        GLPrintExtensions = 0x20,  /*!< Print GL extensions on startup*/
        GLES              = 0x40,  /*!< Request GLES is used*/
        GLSRGB            = 0x100, /*!< Request SRGB framebuffers*/

        GLFeatureLevelProfile = 0x80,

    };
    ContextBits bits;    /*!< Context bits*/
    Flags       flags;   /*!< Context flags*/
    GL::Version version; /*!< Context version*/
};

C_FLAGS(GL::Properties::Flags, libc_types::u8);

} // namespace GL
} // namespace Display
} // namespace Coffee
