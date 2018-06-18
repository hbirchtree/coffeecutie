#pragma once

#include "../../plat_primary_identify.h"

#ifdef COFFEE_ANDROID

#include "../unix/loader.h"

namespace Coffee{
namespace Library{
namespace Android{

using AndroidFunctionLoader = Posix::PosixFunctionLoader;
using AndroidObjectLoader = Posix::PosixObjectLoader;

}

using FunctionLoader = Android::AndroidFunctionLoader;
using ObjectLoader = Android::AndroidObjectLoader;

}
}
#endif
