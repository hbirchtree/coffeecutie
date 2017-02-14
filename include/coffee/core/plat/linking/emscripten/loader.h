#pragma once

#if defined(__EMSCRIPTEN__) || defined(COFFEE_NACL)

#include "../libraries.h"

namespace Coffee{
namespace Library {

using FunctionLoader = FunctionLoad_def;
using ObjectLoader = ObjectLoader_def<FunctionLoader>;

}
}
#endif
