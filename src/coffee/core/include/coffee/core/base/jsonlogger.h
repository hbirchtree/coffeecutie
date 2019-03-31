#pragma once

#include <coffee/core/base/printing/log_interface.h>
#include <url/url.h>

namespace Coffee {

extern DebugFun::LogInterface SetupJsonLogger(
    platform::url::Url const& jsonFilename);
}
