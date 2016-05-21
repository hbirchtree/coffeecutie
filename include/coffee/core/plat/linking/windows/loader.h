#pragma once

#include "../../plat_primary_identify.h"

#ifdef COFFEE_WINDOWS

#include "../libraries.h"

namespace Coffee {
namespace Library {
namespace Windows {

struct WindowsFunctionLoader : FunctionLoad_def
{
    struct Library
    {
        HINSTANCE libraryHandle;
    };

    Library* GetLibrary(
            cstring name,
            LoadFlags lfl = NoFlags,
            Version const* ver = nullptr,
            CString* = nullptr)
    {
        Library* l = new Library;

        CString libname = name;
        name += ".dll";

        HINSTANCE hnd = LoadLibrary(libname.c_str());

        if(!l->libraryHandle)
        {
            delete l;
            return nullptr;
        }

        return l;
    }
};
struct WindowsObjectLoader : ObjectLoader_def<WindowsFunctionLoader>
{

};

}

using FunctionLoader = Windows::WindowsFunctionLoader;
using ObjectLoader = Windows::WindowsObjectLoader;

}
}

#endif
