#pragma once

#include <string>

#include <peripherals/libc/types.h>
#include <peripherals/stl/type_safety.h>

#if !defined(COFFEE_NO_ENDIAN_OPS)
#include <peripherals/libc/endian_ops.h>
#endif

#include "../common/endian.h"
#include "../common/tag_class.h"

namespace blam {

using namespace libc_types;

FORCEDINLINE bool tag_class_cmp(tag_class_t v1, tag_class_t v2)
{
    // Tag classes are stored in the map as a little-endian FourCC, while the
    // enum literals are the host-order value. from_le() normalises the
    // map-read value to host order (no-op on little-endian hosts); the raw
    // comparison covers literal-vs-literal use.
    auto v1_i = from_le(C_CAST<u32>(v1));
    auto v2_i = C_CAST<u32>(v2);

    return v1_i == v2_i || v1 == v2;
}

FORCEDINLINE std::string to_string(tag_class_t tcl)
{
    using namespace std::string_literals;
    if(tcl == tag_class_t::undefined)
        return "undefined"s;
    std::string casted(reinterpret_cast<const char*>(&tcl), 4);
    if(auto idx = casted.find('\0'); idx != std::string::npos)
        casted.resize(idx);
    return casted;
}

#if !defined(COFFEE_MSVCXX)
FORCEDINLINE bool operator==(tag_class_t t1, tag_class_t t2)
{
    return tag_class_cmp(t1, t2);
}

FORCEDINLINE bool operator!=(tag_class_t t1, tag_class_t t2)
{
    return !(t1 == t2);
}
#endif

} // namespace blam
