#pragma once

#include "../../plat_primary_identify.h"

#if defined(COFFEE_LINUX) || defined(COFFEE_APPLE)

#include "../libraries.h"
#include "../../memory/stlstring_ops.h"

/*dlopen,dlsym,dlerror,dlclose*/
#include <dlfcn.h>

namespace Coffee{
namespace Library{
namespace Linux{

struct PosixFunctionLoader : FunctionLoad_def
{
#if defined(COFFEE_LINUX)
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

    STATICINLINE
    Library* GetLibrary(
            cstring libName,
            LoadFlags flags = NoFlags,
            Version const* ver = nullptr,
            CString* err = nullptr)
    {
        CString perm[4];
#if defined(COFFEE_LINUX)
        perm[0] = "lib"+CString(libName)+shared_object_extension;
#elif defined(COFFEE_APPLE)
        perm[0] = "lib"+CString(libName);
#else
        perm[0] = "lib"+CString(libName)+shared_object_extension;
#endif
        if(ver)
        {
#if defined(COFFEE_LINUX)
            perm[1] = perm[0]+Mem::Convert::uinttostring(ver->major);
            perm[2] = perm[1]+Mem::Convert::uinttostring(ver->minor);
            perm[3] = perm[2]+Mem::Convert::uinttostring(ver->revision);
#elif defined(COFFEE_APPLE)
            perm[1] = perm[0]+Mem::Convert::uinttostring(ver->major);
            perm[2] = perm[1]+Mem::Convert::uinttostring(ver->minor);
            perm[3] = perm[2]+Mem::Convert::uinttostring(ver->revision)+shared_object_extension;

            perm[0] = perm[0]+shared_object_extension;
            perm[1] = perm[1]+shared_object_extension;
            perm[2] = perm[2]+shared_object_extension;
#endif
        }

        szptr i=0;
        void* handle = nullptr;

        while(!handle && i<4)
        {
            handle = dlopen(perm[i].c_str(),RTLD_NOW|flags);
            i++;
        }

        if(!handle)
        {
            if(err)
                *err = LinkError();

            return nullptr;
        }else
            return new Library{handle};
    }
    STATICINLINE
    bool UnloadLibrary(Library* lib, CString* err = nullptr)
    {
        bool stat = dlclose(lib->handle)==0;
        if(!stat && err)
            *err = LinkError();
        return stat;
    }

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

using FunctionLoader = Linux::PosixFunctionLoader;
using ObjectLoader = Linux::PosixObjectLoader;

}
}
#endif
