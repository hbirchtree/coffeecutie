#pragma once

#include "../../plat_primary_identify.h"

#if defined(COFFEE_LINUX) || defined(COFFEE_APPLE) || defined(COFFEE_ANDROID)

#include "../libraries.h"
#include "../../memory/stlstring_ops.h"

/*dlopen,dlsym,dlerror,dlclose*/
#include <dlfcn.h>

namespace Coffee{
namespace Library{
namespace Posix{

struct PosixFunctionLoader : FunctionLoad_def
{
#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID)
    static const constexpr cstring shared_object_extension = ".so";
#elif defined(COFFEE_APPLE)
    static const constexpr cstring shared_object_extension = ".dylib";
#endif

    STATICINLINE
    cstring LinkError()
    {
        return dlerror();
    }

    enum LoadFlags
    {
        NoFlags,
        GlobalSymbols = RTLD_GLOBAL,
    };

    struct Library
    {
        void* handle;
    };

    static Library* GetLibrary(
            cstring libName,
            LoadFlags flags = NoFlags,
            Version const* ver = nullptr,
            CString* err = nullptr);

    static bool UnloadLibrary(Library* lib, CString* err = nullptr);

    template<typename RType, typename... Args>
    struct Loader : FunctionLoad_def::Loader<RType,Args...>
    {
        using Fun = RType(*)(Args...);

        STATICINLINE
        Fun GetFunction(
                Library* lib,
                cstring funcName,
                CString* err = nullptr)
        {
            void* sym = dlsym(lib->handle,funcName);
            if(!sym)
            {
                if(err)
                    *err = LinkError();
                return nullptr;
            }else
                return (Fun)sym;
        }
    };
};

struct PosixObjectLoader : ObjectLoader_def<PosixFunctionLoader>
{
    using Loader = PosixFunctionLoader;

    template<typename Obj,typename... Args> STATICINLINE
    ObjConstructor<Obj> GetConstructor(
            Loader::Library* library,
            cstring constructor,
            CString* err = nullptr)
    {
        return {Loader::Loader<Obj*,Args...>::GetFunction(library,constructor,err)};
    }
};



}

using FunctionLoader = Posix::PosixFunctionLoader;
using ObjectLoader = Posix::PosixObjectLoader;

}
}
#endif
