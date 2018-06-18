#include "environment/argument_parse.h"

#include "environment/android/environment.h"
#include "environment/emscripten/environment.h"
#include "environment/linux/environment.h"
#include "environment/unix/environment.h"
#include "environment/osx/environment.h"
#include "environment/windows/environment.h"

#include "plat_process.h"
#include "plat_stacktrace.h"
#include "plat_sysinfo.h"

namespace Coffee{

using Env = Env_;
using ColorMap = ColorMap_;

}
