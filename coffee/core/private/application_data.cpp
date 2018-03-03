#include <coffee/core/coffee_version.h>

namespace Coffee{

#if defined(COFFEE_ANDROID)
CString plat_tmp_string;
#endif

/* Information that is not otherwise available */
cstring CoffeeCompilerString = C_COMPILER_DEFINE_STRING(C_COMPILER_NAME,
                                                        C_COMPILER_VER_MAJ,
                                                        C_COMPILER_VER_MIN,
                                                        C_COMPILER_VER_REV);
cstring CoffeeArchString = COFFEE_ARCH;
cstring CoffeeBuildString = COFFEE_BUILD_STRING;

cstring CoffeePlatformString = C_SYSTEM_STRING;

CString CoffeeDefaultWindowName;

#if 0
#ifndef COFFEE_LOADABLE_LIBRARY
extern CoffeeApplicationData app_data;
#endif

const CoffeeApplicationData &ApplicationData()
{
#ifdef COFFEE_LOADABLE_LIBRARY
    static CoffeeApplicationData app_data;
#endif
    return app_data;
}
#endif

}
