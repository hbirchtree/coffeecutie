#ifndef COFFEE_PLAT_LIBRARIES_H
#define COFFEE_PLAT_LIBRARIES_H

#include "../../coffee_assert_macros.h"
#include "../../types/tdef/integertypes.h"
#include "../../types/cdef/infotypes.h"
#include "../platform_detect.h"

namespace Coffee{
namespace Library{

struct FunctionLoad_def
{
    enum LoadFlags
    {
        NoFlags,
        GlobalSymbols,
    };

    struct Library;

    STATICINLINE
    /*!
     * \brief Load a dynamic library into memory
     * \param name Name of the library, eg. "test" for libtest.so
     * \param flags Options for library loading
     * \param ver Specify version of library
     * \return Pointer to the loaded library, platform-specific data
     */
    Library* GetLibrary(
            cstring name,
            LoadFlags flags = NoFlags,
            Version const* ver = nullptr)
    {
        C_UNUSED(name);
        C_UNUSED(flags);
        C_UNUSED(ver);
        return nullptr;
    }
    STATICINLINE
    /*!
     * \brief Unload a loaded library, platform-dependent definition
     * \param lib
     * \return
     */
    bool UnloadLibrary(Library* lib)
    {
        C_UNUSED(lib);
        return false;
    }

    template<typename RType, typename... Args>
    struct Loader
    {
        using Fun = RType(*)(Args...);

        STATICINLINE
        /*!
         * \brief Load a function from an external library
         * \param lib Source library
         * \param funcName Name of function to load
         * \return
         */
        Fun GetFunction(
                Library* lib,
                cstring funcName)
        {
            C_UNUSED(lib);
            C_UNUSED(funcName);
            return nullptr;
        }
    };
};

template<typename FunctionLoader>
struct ObjectLoader_def
{
    template<typename Obj,typename... Args>
    struct ObjConstructor
    {
        using LoaderFunction = Obj*(*)(Args...);

        LoaderFunction loader;
    };

    template<typename Obj,typename... Args> STATICINLINE
    /*!
     * \brief Load an object constructor from a file
     * \param library Loaded library which should contain the function
     * \param constructor Name of constructor to load
     * \return
     */
    ObjConstructor<Obj> GetConstructor(
            typename FunctionLoader::Library* library,
            cstring constructor)
    {
        C_UNUSED(library);
        C_UNUSED(constructor);
        return {};
    }
};

const constexpr cstring DefaultConstructorFunction = "CoffeeLoader";

}

//namespace CLibraryLoader{

//struct CNativeObject;

//template<typename LibInterface>
//struct CObjectLoader
//{
//    typedef LibInterface*(*LoaderFunction)();

//    CNativeObject* native;
//    LoaderFunction loader;
//};

///*!
// * \brief Acquire function pointer from a loaded native library object
// * \param object
// * \return
// */
//extern void* _coffee_get_funptr(CNativeObject* object);
///*!
// * \brief Loads library object from file to memory, allowing it to be used. Will resolve all symbols if platform allows it.
// * \param file
// * \param loaderFunction
// * \return Valid pointer if object was successfully loaded, null if it failed
// */
//extern CNativeObject *_coffee_get_library(cstring file, cstring loaderFunction, _cbasic_version<int32> const* libver);
//extern void _coffee_close_library(CNativeObject* object);

//template<typename LibInterface, cstring loaderFunction = nullptr>
///*!
// * \brief If successful, loads a library implementation into the program.
// * \param file Name of the library without platform-specific appendages such as file extensions or "lib"
// * \return Valid pointer if it succeeded, null if it failed
// */
//CObjectLoader<LibInterface>* GetLib(
//        cstring file,
//        _cbasic_version<int32> const* libver = nullptr)
//{
//    typedef LibInterface*(*LoaderFunction)();

//    CNativeObject* obj = _coffee_get_library(
//                file,
//                ((bool)loaderFunction) ? loaderFunction : "CoffeeLoader",
//                libver);

//    if(!obj)
//        return nullptr;

//    CObjectLoader<LibInterface>* lc = new CObjectLoader<LibInterface>;

//    lc->native = obj;
//    lc->loader = (LoaderFunction)_coffee_get_funptr(obj);

//    return lc;
//}

///*!
// * \brief Closes the library. Be aware that it might have an effect on running code.
// */
//template<typename LibInterface> void coffee_close_lib(
//        CObjectLoader<LibInterface>* library)
//{
//    _coffee_close_library(library->native);
//    delete library;
//}

//}
}

#endif

#include "linux/loader.h"