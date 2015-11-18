#include "cblam_mod2.h"

namespace Coffee{
namespace CBlam{

const blam_mod2_header *blam_mod2_get_header(const blam_index_item *item, const blam_file_header *map, int32 magic)
{
    if(!map || !item)
        return nullptr;
    const byte_t* b_ptr = (const byte_t*)map;
    return (const blam_mod2_header*)(b_ptr+item->offset-magic);
}

}
}
