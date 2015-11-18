#include "plat_libraries.h"

#if defined(COFFEE_LINUX)
#include <dlfcn.h>
#elif defined(COFFEE_WINDOWS)
#include <Windows.h>
#endif

namespace Coffee{
namespace CLibraryLoader{

constexpr cstring lib_load_error_format = "Native library loading error: %s";
constexpr cstring lib_symb_error_format = "Native library symbol resolution error: %s";

#if defined(COFFEE_LINUX)
struct CNativeObject
{
    void* handle;
    void* funptr;
};

CNativeObject* _coffee_get_library(cstring file, cstring loaderFunction)
{
    CNativeObject *e = new CNativeObject;

    e->handle = dlopen(file,RTLD_NOW);
    if(!e->handle)
    {
        cWarning(lib_load_error_format,dlerror());
        _coffee_close_library(e);
        return nullptr;
    }

    byte_t* error = nullptr;
    e->funptr = dlsym(e->handle,loaderFunction);
    if((error = dlerror()) != NULL)
    {
        cWarning(lib_symb_error_format,error);
        _coffee_close_library(e);
        return nullptr;
    }

    return e;
}

void _coffee_close_library(CNativeObject* object)
{
    if(object->handle)
        dlclose(object->handle);
    delete object;
}

void* _coffee_get_funptr(CNativeObject* object)
{
    return object->funptr;
}
#endif

#if defined(COFFEE_WINDOWS)

struct CNativeObject
{
    HINSTANCE hinstLib;
    void* procedure;
};

CNativeObject* _coffee_get_library(cstring file, cstring loaderFunction)
{
    CNativeObject* e = new CNativeObject;

    e->hinstLib = LoadLibrary(file);

    if(!e->hinstLib)
    {
        cWarning(lib_load_error_format,file);
        _coffee_close_library(e);
        return nullptr;
    }

    e->procedure = GetProcAddress(e->hinstLib,loaderFunction);

    if(!e->procedure)
    {
        cWarning(lib_symb_error_format,file);
        _coffee_close_library(e);
        return nullptr;
    }

    return e;
}

void _coffee_close_library(CNativeObject* library)
{
    if(library->hinstLib)
        FreeLibrary(library->hinstLib);
    delete library;
}

void* _coffee_get_funptr(CNativeObject *object)
{
    return object->procedure;
}
#endif

}
}
