#include <coffee/core/CUnitTesting>
#include <coffee/core/CFiles>
#include <coffee/core/plat/plat_linking.h>
#include "library.h"

using namespace Coffee;
using namespace Library;

const constexpr cstring TargetLibrary = "EnvLibraryLoadTester";

bool dlopen_test()
{
    /* Try loading a shared library, remember to clean up! */
    Version libver;
    libver.major = 1;
    libver.minor = 0;
    libver.revision = 0;

    CString error;

    auto testlib = FunctionLoader::GetLibrary(
                TargetLibrary,
                FunctionLoader::NoFlags,
                &libver,
                &error);

    if(!testlib)
    {
        cDebug("Failed to load library: {0}",error);
        return false;
    }

    auto constrct = ObjectLoader::GetConstructor<TestClass>(
                testlib,
                DefaultConstructorFunction,
                &error);

    if(constrct.loader)
    {
        TestClass* impl = constrct.loader();
        impl->printHello();
        FunctionLoader::UnloadLibrary(testlib);
        return true;
    }else
    {
        cDebug("Failed to load function pointer: {0}",error);
        return false;
    }
}

const constexpr CoffeeTest::Test _tests[1] = {
    dlopen_test
};

COFFEE_RUN_TESTS(_tests);