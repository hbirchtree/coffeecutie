#include <blam/elegy/collision.h>

namespace blam::elegy {

std::optional<u32> coll::bsp_view_t::find_leaf(Vecf3 const& p) const
{
    if(nodes.empty())
        return std::nullopt;
    u32 node = 0;
    for(u32 guard = 0; guard < 4096; guard++)
    {
        if(node >= nodes.size())
            return std::nullopt;
        auto const& n = nodes[node];
        if(n.plane() >= planes.size())
            return std::nullopt;
        auto const& plane = planes[n.plane()];
        f32  side  = glm::dot(plane.normal.value(), p) - plane.distance.value();
        u32  child = side >= 0.f ? n.front_child() : n.back_child();
        if(bsp3d_node_t::is_none(child))
            return std::nullopt;
        if(bsp3d_node_t::is_leaf(child))
            return bsp3d_node_t::leaf_index(child);
        node = child;
    }
    return std::nullopt;
}

} // namespace blam::elegy
