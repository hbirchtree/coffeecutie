#ifndef COFFEE_PLAT_LIBRARIES_H
#define COFFEE_PLAT_LIBRARIES_H

#include "coffee/core/CBase"
#include "coffee/core/CTypes"
#include "platform_detect.h"

#if defined(COFFEE_LINUX)
#include <dlfcn.h>
#elif defined(COFFEE_WIN32)

#endif

namespace Coffee{
namespace CLibraryLoader{

struct CNativeObject;

template<typename LibInterface>
struct CObjectLoader
{
    typedef LibInterface*(*LoaderFunction)();

    CNativeObject* native;
    LoaderFunction loader;
};

/*!
 * \brief Acquire function pointer from a loaded native library object
 * \param object
 * \return
 */
extern void* _coffee_get_funptr(CNativeObject* object);
/*!
 * \brief Loads library object from file to memory, allowing it to be used. Will resolve all symbols if platform allows it.
 * \param file
 * \param loaderFunction
 * \return Valid pointer if object was successfully loaded, null if it failed
 */
extern CNativeObject *_coffee_get_library(cstring file, cstring loaderFunction);
extern void _coffee_close_library(CNativeObject* object);

template<typename LibInterface, cstring loaderFunction = nullptr>
CObjectLoader<LibInterface>* coffee_get_lib(cstring file)
{
    typedef LibInterface*(*LoaderFunction)();

    CNativeObject* obj = _coffee_get_library(
                file,
                loaderFunction ? loaderFunction : "CoffeeLoader");

    if(!obj)
        return nullptr;

    CObjectLoader<LibInterface>* lc = new CObjectLoader<LibInterface>;

    lc->native = obj;
    lc->loader = (LoaderFunction)_coffee_get_funptr(obj);

    return lc;
}

/*!
 * \brief Closes the library. Be aware that it might have an effect on running code.
 */
template<typename LibInterface> void coffee_close_lib(
        CObjectLoader<LibInterface>* library)
{
    _coffee_close_library(library->native);
    delete library;
}

}
}

#endif
