#pragma once

#include "blam_base_types.h"
#include "blam_reflexive.h"
#include "blam_strings.h"
#include "blam_tag_index.h"

namespace blam {

template<typename V>
struct multiplayer_scenario
{
    struct map_string
    {
        reflexive_t<byte_t, V> unknown_1;
        u32                    unknown_2;
        u32                    unknown_3;
        unicode_reflexive<V>   unknown_4;

        u16 data[32];
    };

    struct level_ref
    {
        tagref_typed_t<tag_class_t::bitm> thumbnail;
        tagref_typed_t<tag_class_t::ustr> name;

        bl_string_var<36> map_name;
    };

    reflexive_t<level_ref, V> unknown_data;
};

}
