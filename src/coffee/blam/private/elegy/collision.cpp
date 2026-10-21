#include <blam/elegy/collision.h>

namespace blam::elegy {

std::optional<u32> coll::bsp_view_t::find_leaf(Vecf3 const& p) const
{
    return collision_bsp::find_leaf<traits>(nodes, planes, p, 4096);
}

} // namespace blam::elegy
