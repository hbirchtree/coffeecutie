#include <coffee/core/CUnitTesting>
#include <coffee/core/CFiles>
#include <coffee/core/plat/plat_linking.h>
#include "library.h"

using namespace Coffee;
using namespace CLibraryLoader;

bool dlopen_test()
{
    /* Try loading a shared library, remember to clean up! */
    _cbasic_version<int32> libver;
    libver.major = 1;
    libver.minor = 0;
    libver.revision = 0;
    auto libtest = GetLib<TestClass>("EnvLibraryLoadTester",&libver);

    if(libtest)
    {
        TestClass* impl = libtest->loader();
        impl->printHello();
        coffee_close_lib(libtest);
        return true;
    }
    return false;
}

static const constexpr CoffeeTest::Test _tests[1] = {
    dlopen_test
};

COFFEE_RUN_TESTS(_tests);