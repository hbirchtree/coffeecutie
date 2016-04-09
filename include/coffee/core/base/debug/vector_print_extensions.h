#pragma once

#include "extensionresolvers.h"
#include "../../types/vector_types.h"

namespace Coffee{
namespace DebugFun{

inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        CQuat const& v)
{
    return extArgReplace(fmt,index,cStringFormat("q({0}:{1},{2},{3})",
                                               v.w(),v.x(),v.y(),v.z()));
}

inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        CVec4 const& v)
{
    return extArgReplace(fmt,index,cStringFormat("v({0},{1},{2},{3})",
                                               v.x(),v.y(),v.z(),v.w()));
}

inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        CVec3 const& v)
{
    return extArgReplace(fmt,index,cStringFormat("v({0},{1},{2})",
                                               v.x(),v.y(),v.z()));
}

inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        CVec2 const& v)
{
    return extArgReplace(fmt,index,cStringFormat("v({0},{1})",
                                               v.x(),v.y()));
}

inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        CMat4 const& v)
{
    return extArgReplace(
                fmt,index,
                cStringFormat("m({0},{1},{2},{3}})",
                              v[0],v[1],v[2],v[3]));
}

inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        CMat3 const& v)
{
    return extArgReplace(
                fmt,index,
                cStringFormat("m({0},{1},{2}})",
                              v[0],v[1],v[2]));
}

inline CString cStringReplace(
	CString const& fmt, size_t const& index,
	BoundBox const& box)
{
    return extArgReplace(
		fmt,index,
		cStringFormat(
		    "b({0}->{1},{2}->{3},{4}->{5})",
		    box.x1(),box.x2(),
		    box.y1(),box.y2(),
		    box.z1(),box.z2()));
}

}
}
