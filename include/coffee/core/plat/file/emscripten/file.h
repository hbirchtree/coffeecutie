#pragma once

#ifndef __emscripten__

#include "../cfile.h"

namespace Coffee{
namespace CResources{

using DirFun = DirFunDef;
using FileFun = CFILEFun_def<FILEApi::FileHandle>;

}
}
#endif
