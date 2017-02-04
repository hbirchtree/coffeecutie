#include "environment/argument_parse.h"

#include "environment/android/environment.h"
#include "environment/emscripten/environment.h"
#include "environment/linux/environment.h"
#include "environment/osx/environment.h"
#include "environment/windows/environment.h"

#include "environment/android/process.h"
#include "environment/emscripten/process.h"
#include "environment/linux/process.h"
#include "environment/osx/process.h"
#include "environment/windows/process.h"

#include "environment/android/stacktrace.h"
#include "environment/emscripten/stacktrace.h"
#include "environment/linux/stacktrace.h"
#include "environment/osx/stacktrace.h"
#include "environment/windows/stacktrace.h"

#include "environment/android/sysinfo.h"
#include "environment/emscripten/sysinfo.h"
#include "environment/linux/sysinfo.h"
#include "environment/osx/sysinfo.h"
#include "environment/raspberry/sysinfo.h"
#include "environment/windows/sysinfo.h"

namespace Coffee{

using Env = Env_;
using ColorMap = ColorMap_;

}
