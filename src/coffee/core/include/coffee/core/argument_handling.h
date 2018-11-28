#pragma once

#include "types/cdef/infotypes.h"

namespace Coffee{

extern platform::info::AppArg& GetInitArgs();

namespace Strings{

extern CString to_string(AppArg const& args);

}
}
