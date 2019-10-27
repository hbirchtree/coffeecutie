#include <peripherals/build/build_info.h>

#include <peripherals/identify/architecture.h>
#include <peripherals/identify/compiler.h>
#include <peripherals/stl/types.h>

#include "module_version.h"

namespace platform {
namespace info {

const cstring architecture      = COFFEE_ARCH;
const cstring build_version     = MODULE_VERSION;
const cstring platform_identity = C_SYSTEM_STRING;
const cstring compiler          = C_COMPILER_DEFINE_STRING(
    C_COMPILER_NAME,
    C_COMPILER_VER_MAJ,
    C_COMPILER_VER_MIN,
    C_COMPILER_VER_REV);

const cstring build_mode =
        #if MODE_DEBUG
        "Debug"
        #elif MODE_LOWFAT
        "Low-fat"
        #elif MODE_RELEASE
        "Release"
        #else
        "Undefined"
        #endif
        ;

} // namespace info
} // namespace platform
