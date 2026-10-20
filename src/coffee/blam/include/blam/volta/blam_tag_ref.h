#pragma once

#include "../common/tag_reference.h"
#include "blam_strings.h"
#include "blam_tag_classes.h"
#include "blam_versions.h"

namespace blam {

/*! Halo 1: class, name, a runtime word and the handle; null when unset */
template<tag_class_t... Classes>
struct tagref_storage<layout::volta_pc, Classes...>
{
    basic_tag_class<std::endian::little> tag_class{{0xFF, 0xFF, 0xFF, 0xFF}};
    string_ref                           name;
    i32                                  unknown{0};
    tag_index<Classes...>                tag_id{0xFFFFFFFF};
};

template<tag_class_t... Classes>
struct tagref_storage<layout::volta_xbox, Classes...>
    : tagref_storage<layout::volta_pc, Classes...>
{
};

template<tag_class_t... Tags>
using tagref_typed_t = tag_reference<pc_version_t, Tags...>;

using tagref_t = tagref_typed_t<>;

static_assert(sizeof(tagref_t) == 16);

} // namespace blam
