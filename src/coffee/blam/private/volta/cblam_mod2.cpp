#include <coffee/blam/volta/cblam_mod2.h>

namespace blam {
namespace mod2 {

const header* blam_mod2_get_header(
    const index_item_t* item, const file_header_t* map, i32 magic)
{
    if(!map || !item)
        return nullptr;
    byte_t const* b_ptr = C_RCAST<byte_t const*>(map);
    return C_RCAST<header const*>(b_ptr + item->offset - magic);
}

} // namespace mod2
} // namespace blam
