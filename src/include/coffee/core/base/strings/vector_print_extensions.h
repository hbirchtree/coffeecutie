#pragma once

#include "../../types/vector_types.h"
#include "../../types/cdef/geometry.h"
#include "../../types/tdef/stltypes.h"

namespace Coffee{
namespace Strings{

extern CString to_string(CQuat const& v);

extern CString to_string(CVec4 const& v);

extern CString to_string(CVec3 const& v);

extern CString to_string(CVec2 const& v);

extern CString to_string(CMat4 const& v);

extern CString to_string(CMat3 const& v);

extern CString to_string(BoundBox const& box);

}
}
