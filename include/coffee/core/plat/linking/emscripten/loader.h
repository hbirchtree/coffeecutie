#pragma once

#ifdef __emscripten__

#include "../libraries.h"

namespace Coffee{
namespace Library {

using FunctionLoader = FunctionLoad_def;
using ObjectLoader = ObjectLoader_def<FunctionLoader>;

}
}
#endif
