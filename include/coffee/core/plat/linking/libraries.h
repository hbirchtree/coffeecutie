#ifndef COFFEE_PLAT_LIBRARIES_H
#define COFFEE_PLAT_LIBRARIES_H

#include "../../types/tdef/integertypes.h"
#include "../../types/cdef/infotypes.h"
#include "../platform_detect.h"

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
extern CNativeObject *_coffee_get_library(cstring file, cstring loaderFunction, _cbasic_version<int32> const* libver);
extern void _coffee_close_library(CNativeObject* object);

template<typename LibInterface, cstring loaderFunction = nullptr>
/*!
 * \brief If successful, loads a library implementation into the program.
 * \param file Name of the library without platform-specific appendages such as file extensions or "lib"
 * \return Valid pointer if it succeeded, null if it failed
 */
CObjectLoader<LibInterface>* GetLib(
        cstring file,
        _cbasic_version<int32> const* libver = nullptr)
{
    typedef LibInterface*(*LoaderFunction)();

    CNativeObject* obj = _coffee_get_library(
                file,
                ((bool)loaderFunction) ? loaderFunction : "CoffeeLoader",
                libver);

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
