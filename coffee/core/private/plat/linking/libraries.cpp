#include <plat/linking/libraries.h>

#include <coffee_strings.h>

#if defined(COFFEE_LINUX)
#include <dlfcn.h>
#elif defined(COFFEE_WINDOWS)
#include <Windows.h>
#endif

namespace Coffee{
namespace CLibraryLoader{

#if defined(COFFEE_LINUX)
struct CNativeObject
{
    void* handle;
    void* funptr;
};

void* _coffee_dlopen(cstring fname)
{
    void* handle = dlopen(fname,RTLD_NOW);
    if(!handle)
    {
        cLog(__FILE__,__LINE__,"CObjectLoader",CFStrings::Lib_load_error_format,fname);
    }
    return handle;
}

CNativeObject* _coffee_get_library(cstring file, cstring loaderFunction,
                                   const _cbasic_version<int32> *libver)
{
    CNativeObject *e = new CNativeObject;

    CString plat_file_name = "lib";
    plat_file_name += file;
    plat_file_name += ".so";

    e->handle = _coffee_dlopen(plat_file_name.c_str());

    if(libver&&!e->handle)
    {
        plat_file_name.append(cStringFormat(
                    ".{0}.{1}.{2}",
                    libver->major,
                    libver->minor,
                    libver->revision));
        e->handle = _coffee_dlopen(plat_file_name.c_str());
    }

    if(!e->handle)
    {
        delete e;
        return nullptr;
    }

    cstring error = nullptr;
    e->funptr = dlsym(e->handle,loaderFunction);
    if((error = dlerror()) != NULL)
    {
        cLog(__FILE__,__LINE__,CFStrings::Lib_Identifier,
             CFStrings::Lib_symb_error_format,error);
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

CNativeObject* _coffee_get_library(cstring file, cstring loaderFunction,
                                   const _cbasic_version<int32> *libver)
{
    CNativeObject* e = new CNativeObject;

    CString plat_file_name = file;
    plat_file_name += ".dll";

    e->hinstLib = LoadLibrary(plat_file_name.c_str());

    if(!e->hinstLib)
    {
        cWarning(CFStrings::Lib_load_error_format,plat_file_name.c_str());
        _coffee_close_library(e);
        return nullptr;
    }

    e->procedure = GetProcAddress(e->hinstLib,loaderFunction);

    if(!e->procedure)
    {
        cWarning(CFStrings::Lib_symb_error_format,plat_file_name.c_str());
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
