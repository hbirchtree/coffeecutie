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
        HINSTANCE libraryHandle;
    };

	STATICINLINE
	Library* GetLibrary(
            cstring name,
            LoadFlags lfl = NoFlags,
            Version const* ver = nullptr,
            CString* = nullptr)
    {
        Library* l = new Library;

        CString libname = name;
        libname += LIBRARY_SUFFIX;

        HINSTANCE hnd = LoadLibrary(libname.c_str());

        if(!l->libraryHandle)
        {
            delete l;
            return nullptr;
        }

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
				CString* = nullptr)
		{
			return (Fun)GetProcAddress(lib->libraryHandle,funcname);
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
