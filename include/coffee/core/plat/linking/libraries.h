#pragma once

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
            Version const* ver = nullptr,
            CString* = nullptr)
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
    bool UnloadLibrary(Library* lib,
                       CString* = nullptr)
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
                cstring funcName,
                CString*)
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
            cstring constructor,
            CString* = nullptr)
    {
        C_UNUSED(library);
        C_UNUSED(constructor);
        return {};
    }
};

const constexpr cstring DefaultConstructorFunction = "CoffeeLoader";

}
}
