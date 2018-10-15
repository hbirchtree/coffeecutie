#pragma once

#include "../../types/vector_types.h"
#include "../../types/cdef/geometry.h"
#include "../../types/tdef/stltypes.h"

namespace Coffee{
namespace Strings{

extern CString to_string(Quatf const& v);

extern CString to_string(Vecf4 const& v);

extern CString to_string(Vecf3 const& v);

extern CString to_string(Vecf2 const& v);

extern CString to_string(Matf4 const& v);

extern CString to_string(Matf3 const& v);

extern CString to_string(BoundBox const& box);

}
}
