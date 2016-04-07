#include "environment/argument_parse.h"

#include "environment/environment_details.h"
#include "environment/android/environment.h"
#include "environment/linux/environment.h"
#include "environment/windows/environment.h"

#include "environment/process_def.h"
#include "environment/android/process.h"
#include "environment/linux/process.h"
#include "environment/windows/process.h"

#include "environment/stacktrace_def.h"
#include "environment/linux/stacktrace.h"
#include "environment/android/stacktrace.h"

#include "environment/sysinfo_def.h"
#include "environment/android/sysinfo.h"
#include "environment/linux/sysinfo.h"
#include "environment/windows/sysinfo.h"

namespace Coffee{

using Env = Env_;
using ColorMap = ColorMap_;

}
