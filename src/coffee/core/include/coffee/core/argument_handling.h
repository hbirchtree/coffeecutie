#pragma once

#include <coffee/core/stl_types.h>
#include <platforms/argument_parse.h>

namespace Coffee{

extern platform::args::AppArg& GetInitArgs();

namespace Strings{

extern CString to_string(platform::args::AppArg const& args);

}
}
