#ifndef COFFEE_PLAT_LIBRARIES_H
#define COFFEE_PLAT_LIBRARIES_H

#include "coffee/core/Base"
#include "coffee/core/Types"
#include "platform_detect.h"

#if defined(COFFEE_LINUX)
#include <dlfcn.h>
#elif defined(COFFEE_WIN32)

#endif

namespace Coffee{
namespace CLibraryLoader{

struct CLoadedLibrary
{
    void* object;
};

#if defined(COFFEE_LINUX)
template<typename LibInterface>
LibInterface* coffee_get_lib(cstring file)
{
    void* object = dlopen(file,RTLD_LAZY);
    if(!object)
    {
	cFatal("Failed to load library from file: %s",file);
    }
    byte* error = nullptr;
    LibInterface* (*loader)() = dlsym(object,"CoffeeLoader");
    if((error == dlerror()) != NULL)
    {
	cFatal("Error while loading library: %s",error);
    }
}

void coffee_close_lib(void* object)
{
    dlclose(object);
}

#endif

}
}

#endif
