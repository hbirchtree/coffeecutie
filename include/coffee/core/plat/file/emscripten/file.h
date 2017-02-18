#pragma once

#if defined(COFFEE_STUBBED_CFILE)

#include "../cfile.h"

namespace Coffee{
namespace CResources{

struct EmscriptenFileFun : CFILEFun_def<FILEApi::FileHandle>
{
    using FileHandle = FILEApi::FileHandle;
};

using DirFun = DirFunDef;
using FileFun = EmscriptenFileFun;

}
}
#endif
