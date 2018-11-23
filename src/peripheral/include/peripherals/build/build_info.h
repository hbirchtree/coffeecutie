#pragma once

#ifdef __CBUILDTIME__
#define __CBDATETIME__ __CBUILDTIME__
#else
#define __CBDATETIME__ "00.00.00.0000"
#endif

#define C_CONSTRUCT_BUILD_STRING(ver, datetime) ver "." datetime

#include <peripherals/libc/types.h>

namespace platform {
namespace info {

using namespace ::libc_types;

extern const cstring architecture;
extern const cstring build_version;
extern const cstring compiler;
extern const cstring platform;

} // namespace info
} // namespace platform
