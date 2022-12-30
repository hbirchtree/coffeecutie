#pragma once

#include <coffee/core/libc_types.h>
#include <coffee/core/stl_types.h>

namespace compo::timedivide {

struct node_association
{
    size_t nodes[2]; /*!< The indices of the nodes */
    size_t tag_type; /*!< Hash code of the tag that associates them */
};

}
