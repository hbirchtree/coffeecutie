#pragma once

#include <coffee/core/base/files/url.h>
#include <coffee/core/base/printing/log_interface.h>

namespace Coffee{

extern DebugFun::LogInterface SetupJsonLogger(Url const& jsonFilename);

}
