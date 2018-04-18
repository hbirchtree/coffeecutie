#include <coffee/core/coffee_version.h>
#include <coffee/core/internal_state.h>

namespace Coffee{

void SetBuildInfo(BuildInfo& binfo)
{
    /* Information that is not otherwise available */
    binfo.compiler =
            C_COMPILER_DEFINE_STRING(C_COMPILER_NAME,
                                     C_COMPILER_VER_MAJ,
                                     C_COMPILER_VER_MIN,
                                     C_COMPILER_VER_REV);
    binfo.architecture = COFFEE_ARCH;
    binfo.build_version = COFFEE_BUILD_STRING;
    binfo.platform = C_SYSTEM_STRING;
}

}
