#pragma once

#ifdef __emscripten__
#include "../process_def.h"

namespace Coffee{
namespace Environment{
namespace Emscripten{

struct EmProcessProperty : ProcessPropertyDef
{
};

}
}

using ProcessProperty = Environment::Emscripten::EmProcessProperty;

}
#endif
