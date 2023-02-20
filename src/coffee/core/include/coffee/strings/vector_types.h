#pragma once

#include <coffee/core/stl_types.h>
#include <peripherals/typing/vectors/vector_types.h>

namespace Coffee::Strings {

extern CString to_string(typing::vector_types::Quatf const& v);

extern CString to_string(typing::vector_types::Vecf4 const& v);

extern CString to_string(typing::vector_types::Vecf3 const& v);

extern CString to_string(typing::vector_types::Vecf2 const& v);

extern CString to_string(typing::vector_types::Matf4 const& v);

extern CString to_string(typing::vector_types::Matf3 const& v);

} // namespace Coffee::Strings
