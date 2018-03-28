#pragma once

#include "types/cdef/infotypes.h"

namespace Coffee{

extern AppArg initargs;

extern AppArg& GetInitArgs();

namespace Strings{

extern CString to_string(AppArg const& args);

}
}
