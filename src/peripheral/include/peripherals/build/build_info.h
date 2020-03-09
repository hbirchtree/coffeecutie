#pragma once

#ifdef __CBUILDTIME__
#define __CBDATETIME__ __CBUILDTIME__
#else
#define __CBDATETIME__ "00.00.00.0000"
#endif

#define C_CONSTRUCT_BUILD_STRING(ver, datetime) ver "." datetime

#include <peripherals/libc/types.h>

namespace compile_info {

extern const libc_types::cstring engine_version;

} // namespace compile_info
