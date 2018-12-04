#include <peripherals/build/build_info.h>

#include <peripherals/identify/architecture.h>
#include <peripherals/identify/compiler.h>
#include <peripherals/stl/types.h>

namespace platform {
namespace info {

const cstring architecture      = COFFEE_ARCH;
const cstring build_version     = C_STR(COFFEE_BUILD_STRING);
const cstring platform_identity = C_SYSTEM_STRING;
const cstring compiler          = C_COMPILER_DEFINE_STRING(
    C_COMPILER_NAME,
    C_COMPILER_VER_MAJ,
    C_COMPILER_VER_MIN,
    C_COMPILER_VER_REV);

} // namespace info
} // namespace platform
