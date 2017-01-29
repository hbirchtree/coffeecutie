#include <coffee/blam/volta/cblam_scenario_loader.h>

#include <coffee/core/CDebug>
#include <coffee/blam/volta/cblam_map.h>

namespace Coffee{
namespace Blam{

const scenario *blam_scn_get(
        const file_header_t *map, const tag_index_t* tags)
{
    const index_item_t* base = tag_index_get_items(map);

    if(!MemCmp(index_item_type_scnr,base->tagclass[0],4))
        return nullptr;

    cDebug("Scenario name: {0},offset={1}",
           index_item_get_string(base,map,tags),
           base->offset-tags->index_magic);

    const byte_t* b_ptr = (const byte_t*)map;
    return (const scenario*)(b_ptr+base->offset-tags->index_magic);
}

}
}
