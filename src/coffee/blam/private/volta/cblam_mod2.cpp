#include <coffee/blam/volta/cblam_mod2.h>

namespace Coffee{
namespace Blam{

const blam_mod2_header *blam_mod2_get_header(const index_item_t *item, const file_header_t *map, i32 magic)
{
    if(!map || !item)
        return nullptr;
    const byte_t* b_ptr = (const byte_t*)map;
    return (const blam_mod2_header*)(b_ptr+item->offset-magic);
}

}
}
