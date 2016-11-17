#include <coffee/core/CUnitTesting>
#include <coffee/core/CFiles>
#include <coffee/core/plat/plat_linking.h>
#include <coffee/core/plat/plat_environment.h>
#include "library.h"

using namespace Coffee;
using namespace Library;

const constexpr cstring TargetLibrary = "EnvLibraryLoadTester";

static CString _error;
static FunctionLoader::Library* _library_object;

bool dlopen_test()
{
#if defined(COFFEE_LINUX)
    Env::PrependVar("LD_LIBRARY_PATH",CMAKE_LINKING_DIRECTORY);
    cDebug("Prepending {0}",CMAKE_LINKING_DIRECTORY);
#elif defined(COFFEE_APPLE)
    Env::PrependVar("DYLD_LIBRARY_PATH",CMAKE_LINKING_DIRECTORY);
#endif

    /* Try loading a shared library, remember to clean up! */
    Version libver;
    libver.major = 1;
    libver.minor = 0;
    libver.revision = 0;

    _library_object = FunctionLoader::GetLibrary(
                TargetLibrary,
                FunctionLoader::NoFlags,
                &libver,
                &_error);

    if(!_library_object)
    {
        cDebug("Failed to load library: {0}",_error);
        return false;
    }
    return true;
}

static ObjectLoader::ObjConstructor<TestClass> _constructor;

bool dlsym_test()
{
    _constructor = ObjectLoader::GetConstructor<TestClass>(
                _library_object,
                DefaultConstructorFunction,
                &_error);
    if(!_constructor.loader)
    {
        cDebug("Symbol resolution error: {0}",_error);
        return false;
    }else
        return true;
}

bool constructor_test()
{
    TestClass* impl = _constructor.loader();

    if(!impl)
    {
        cDebug("Failed to construct object");
        return false;
    }

    cDebug("User-verifiable test");
    impl->printHello();

    delete impl;

    return true;
}

bool dlclose_test()
{
    return FunctionLoader::UnloadLibrary(_library_object);
}

const constexpr CoffeeTest::Test _tests[4] = {
    {dlopen_test,"Opening a library","Opens a local library",false,true},
    {dlsym_test,"Symbol resolution","Acquires a symbol from the loaded library",false,true},
    {constructor_test,"Object construction from library","Invokes an external constructor for external code execution",false,true},
    {dlclose_test,"Library closing","Closes the local library file",false,true}
};

COFFEE_RUN_TESTS(_tests);
