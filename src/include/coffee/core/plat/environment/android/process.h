#pragma once

#include <coffee/core/base.h>

#ifdef COFFEE_ANDROID

#include "../linux/process.h"

namespace Coffee {
namespace Environment {
namespace Android {
struct AndroidProcessProperty : Linux::LinuxProcessProperty
{
};
} // namespace Android
} // namespace Environment

using ProcessProperty = Environment::Android::AndroidProcessProperty;

} // namespace Coffee
#endif
