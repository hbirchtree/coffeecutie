#pragma once

#include "../../types/vector_types.h"
#include "../../types/cdef/geometry.h"
#include "../../types/tdef/stltypes.h"

namespace Coffee{
namespace DebugFun{

extern CString cStringReplace(
        CString const& fmt, size_t const& index,
        CQuat const& v);

extern CString cStringReplace(
        CString const& fmt, size_t const& index,
        CVec4 const& v);

extern CString cStringReplace(
        CString const& fmt, size_t const& index,
        CVec3 const& v);

extern CString cStringReplace(
        CString const& fmt, size_t const& index,
        CVec2 const& v);

extern CString cStringReplace(
        CString const& fmt, size_t const& index,
        CMat4 const& v);

extern CString cStringReplace(
        CString const& fmt, size_t const& index,
        CMat3 const& v);

extern CString cStringReplace(
	CString const& fmt, size_t const& index,
    BoundBox const& box);

}
}
