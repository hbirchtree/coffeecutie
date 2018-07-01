#pragma once

#include "../../plat_primary_identify.h"

#if defined(COFFEE_APPLE)

#include "../unix/file.h"

namespace Coffee{
namespace CResources{

using FileFun = Posix::PosixFileFun;
using DirFun = Posix::PosixDirFun;

}
}

#endif
