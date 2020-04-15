#pragma once

#include "android/file.h"
#include "libc/file.h"
#include "linux/file.h"
#include "posix/file.h"
#include "win32/file.h"

#include "base/file.h"

namespace platform {
namespace file {

#if defined(COFFEE_EMSCRIPTEN) || defined(COFFEE_GEKKO)
using FileFun = CFILEFun;
using DirFun  = posix::DirFun;
#endif

using file_error = FileFun::file_error;
}
} // namespace platform
