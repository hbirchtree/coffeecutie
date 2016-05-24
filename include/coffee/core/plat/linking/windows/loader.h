#pragma once

#include "../../plat_primary_identify.h"

#ifdef COFFEE_WINDOWS

#include "../libraries.h"

namespace Coffee {
namespace Library {
namespace Windows {

struct WindowsFunctionLoader : FunctionLoad_def
{
	static const constexpr cstring LIBRARY_SUFFIX = ".dll";

    struct Library
    {
        HMODULE libraryHandle;
    };

	STATICINLINE
	Library* GetLibrary(
            cstring name,
            LoadFlags lfl = NoFlags,
            Version const* ver = nullptr,
            CString* = nullptr)
    {

        CString libname = name;
        libname += LIBRARY_SUFFIX;

        HMODULE hnd = LoadLibrary(libname.c_str());

        if(!hnd)
        {
            return nullptr;
        }

		Library* l = new Library;

		l->libraryHandle = hnd;

        return l;
    }

	STATICINLINE
	bool UnloadLibrary(
			Library* lib,
			CString* = nullptr)
	{
		if (lib->libraryHandle)
		{
			FreeLibrary(lib->libraryHandle);
			delete lib;
			return true;
		}
		return false;
	}

	template<typename RType, typename... AType>
	struct Loader : FunctionLoad_def::Loader<RType,AType...>
	{
		STATICINLINE
		Fun GetFunction(
				Library* lib,
				cstring funcname,
				CString* err = nullptr)
		{
			FARPROC proc = GetProcAddress(lib->libraryHandle, funcname);
			if (!proc && err)
				*err = win_strerror(GetLastError());
			return (Fun)proc;
		}
	};
};
struct WindowsObjectLoader : ObjectLoader_def<WindowsFunctionLoader>
{
	using Loader = WindowsFunctionLoader;

	template<typename Obj,typename... Args> STATICINLINE
		ObjConstructor<Obj> GetConstructor(
			Loader::Library* library,
			cstring constructor,
			CString* err = nullptr
		)
	{
		return{ Loader::Loader<Obj*,Args...>::GetFunction(library,constructor,err)};
	}
};

}

using FunctionLoader = Windows::WindowsFunctionLoader;
using ObjectLoader = Windows::WindowsObjectLoader;

}
}

#endif
