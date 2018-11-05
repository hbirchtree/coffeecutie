#pragma once

#include <coffee/core/base.h>

#if defined(COFFEE_APPLE)

#include "../unix/file.h"

namespace Coffee {
namespace CResources {

using FileFun = Posix::PosixFileFun;
using DirFun  = Posix::PosixDirFun;

} // namespace CResources
} // namespace Coffee

#endif
