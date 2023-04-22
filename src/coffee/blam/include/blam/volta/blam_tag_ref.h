#pragma once

#include "blam_strings.h"
#include "blam_tag_classes.h"

namespace blam {

template<tag_class_t... Tags>
struct alignas(4) tagref_typed_t
{
    tag_class_t tag_class;
    string_ref  name;
    i32         unknown{0};
    u32         tag_id;

    inline bool valid() const
    {
        return tag_id != std::numeric_limits<u32>::max();
    }

    inline string_ref to_name() const
    {
        return name;
    }

    inline bool matches(tag_class_t cls) const
    {
        return tag_class == cls;
    }

    inline std::string_view tag_class_name() const
    {
        return std::string_view(reinterpret_cast<const char*>(&tag_class), 4);
    }

    inline tagref_typed_t<> to_plain() const
    {
        return *C_RCAST<tagref_typed_t<> const*>(this);
    }
};

using tagref_t = tagref_typed_t<>;

} // namespace blam
