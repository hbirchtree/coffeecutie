#pragma once

/* JSON for blam handles (journal, state.json): the raw 32-bit value, as the
 * wire and the Python harness have always seen it. Kept out of the blam
 * headers, which know nothing of JSON. */

#include <blam/common/tag_index.h>
#include <peripherals/stl/json.h>

namespace nlohmann {

template<std::endian E, blam::tag_class_t... Classes>
struct adl_serializer<blam::basic_tag_index<E, Classes...>>
{
    static void to_json(json& j, blam::basic_tag_index<E, Classes...> const& id)
    {
        j = id.value();
    }
};

} // namespace nlohmann
