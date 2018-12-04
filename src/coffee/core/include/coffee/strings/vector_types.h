#pragma once

#include <coffee/core/stl_types.h>
#include <coffee/core/types/vector_types.h>

namespace Coffee {
namespace Strings {

extern CString to_string(Quatf const& v);

extern CString to_string(Vecf4 const& v);

extern CString to_string(Vecf3 const& v);

extern CString to_string(Vecf2 const& v);

extern CString to_string(Matf4 const& v);

extern CString to_string(Matf3 const& v);

} // namespace Strings
} // namespace Coffee
