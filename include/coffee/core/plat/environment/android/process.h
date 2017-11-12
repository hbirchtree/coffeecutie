#pragma once

#include "../../plat_primary_identify.h"

#ifdef COFFEE_ANDROID

#include "../linux/process.h"

namespace Coffee{
namespace Environment{
namespace Android{
struct AndroidProcessProperty : Linux::LinuxProcessProperty
{
};
}
}

using ProcessProperty = Environment::Android::AndroidProcessProperty;

}
#endif
