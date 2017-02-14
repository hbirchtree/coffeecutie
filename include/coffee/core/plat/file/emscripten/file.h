#pragma once

#if defined(__EMSCRIPTEN__) || defined(COFFEE_NACL)

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
