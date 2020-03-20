#include <peripherals/build/build_info.h>

#include <peripherals/identify/architecture.h>
#include <peripherals/identify/compiler.h>
#include <peripherals/stl/types.h>

#include "module_version.h"

namespace compile_info {

const libc_types::cstring engine_version = MODULE_VERSION;

} // namespace compile_info
