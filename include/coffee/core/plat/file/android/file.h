#pragma once

#include "../../plat_primary_identify.h"

#ifdef COFFEE_ANDROID

#include "../cfile.h"

#include "../../../coffee_message_macros.h"

namespace Coffee{
namespace CResources{

struct AndroidFileApi
{
    struct FileHandle : FILEApi::FileHandle
    {
    };
};

struct AndroidFileFun : CFILEFun_def<AndroidFileApi::FileHandle>
{
    static CString NativePath(cstring fn);

    STATICINLINE FileMapping Map(cstring,ResourceAccess,szptr,szptr,int*)
    {
        C_STUBBED("No file mapping defined!");
        return {};
    }
    STATICINLINE bool Unmap(FileMapping*)
    {
        C_STUBBED("No file mapping defined!");
        return false;
    }
    STATICINLINE szptr Size(cstring)
    {
        return 0;
    }
    STATICINLINE bool Exists(cstring)
    {
        return false;
    }

};

using FileFun = AndroidFileFun;
using DirFun = DirFunDef;

}
}
#endif