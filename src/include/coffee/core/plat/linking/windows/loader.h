#pragma once

#include "../../plat_primary_identify.h"

#ifdef COFFEE_WINDOWS

#include "../../plat_windows_errors.h"
#include "../libraries.h"

namespace Coffee {
namespace Library {
namespace Windows {

struct WindowsFunctionLoader : FunctionLoad_def
{
    using error_type = Win32::win32_error_code;

    static const constexpr cstring LIBRARY_SUFFIX = ".dll";

    struct Library
    {
        HMODULE libraryHandle;
    };

    STATICINLINE
    UqPtr<Library> GetLibrary(
        cstring     name,
        error_type& ec,
        LoadFlags   lfl = NoFlags,
        Version const*  = nullptr)
    {
        CString libname = name;
        libname += LIBRARY_SUFFIX;

        HMODULE hnd = nullptr;
#ifndef COFFEE_WINDOWS_UWP
        hnd = LoadLibrary(libname.c_str());
#endif

        if(!hnd)
        {
            ec = GetLastError();
            return nullptr;
        }

        UqPtr<Library> l = MkUq<Library>();

        l->libraryHandle = hnd;

        return l;
    }

    STATICINLINE
    bool UnloadLibrary(UqPtr<Library>&& lib, error_type& ec)
    {
        if(lib && lib->libraryHandle)
        {
            FreeLibrary(lib->libraryHandle);
            ec = GetLastError();
            return true;
        }
        return false;
    }

    template<typename RType, typename... AType>
    struct Loader : FunctionLoad_def::Loader<RType, AType...>
    {
        using Fun = RType (*)(AType...);

        STATICINLINE
        Fun GetFunction(UqPtr<Library>& lib, cstring funcname, error_type& ec)
        {
            FARPROC proc = GetProcAddress(lib->libraryHandle, funcname);

            if(!proc)
                ec = GetLastError();

            return (Fun)proc;
        }
    };
};
struct WindowsObjectLoader : ObjectLoader_def<WindowsFunctionLoader>
{
    using Loader = WindowsFunctionLoader;

    template<typename Obj, typename... Args>
    STATICINLINE ObjConstructor<Obj> GetConstructor(
        UqPtr<Loader::Library>& library,
        cstring                 constructor,
        Loader::error_type&     ec)
    {
        return {Loader::Loader<Obj*, Args...>::GetFunction(
            library, constructor, ec)};
    }
};

} // namespace Windows

using FunctionLoader = Windows::WindowsFunctionLoader;
using ObjectLoader   = Windows::WindowsObjectLoader;

} // namespace Library
} // namespace Coffee

#endif
