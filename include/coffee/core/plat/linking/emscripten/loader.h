#pragma once

#if defined(COFFEE_STUBBED_DYNLOADER)

#include "../libraries.h"

namespace Coffee{
namespace Library {

using FunctionLoader = FunctionLoad_def;
using ObjectLoader = ObjectLoader_def<FunctionLoader>;

}
}
#endif
