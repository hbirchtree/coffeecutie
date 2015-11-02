#include "cblam_scenario_loader.h"

#include <coffee/core/base/cdebug.h>
#include "cblam_map.h"

namespace Coffee{
namespace CBlam{

const blam_scenario* blam_scn_get(
        const blam_file_header *map, const blam_tag_index* tags)
{
    const blam_index_item* base = blam_tag_index_get_items(map);

    if(!coffee_cmp_memarea(blam_index_item_type_scnr,base->tagclass[0],4))
        return nullptr;

    cDebug("Scenario name: %s,offset=%lld",
           blam_index_item_get_string(base,map,tags),
           base->offset-tags->index_magic);

    const byte* b_ptr = (const byte*)map;
    return (const blam_scenario*)(b_ptr+base->offset-tags->index_magic);
}

}
}
