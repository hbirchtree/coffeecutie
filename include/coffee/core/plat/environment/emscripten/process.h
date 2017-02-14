#pragma once

#if defined(__EMSCRIPTEN__) || defined(COFFEE_NACL)

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
