#pragma once

#include <blam/elegy/map.h>
#include <coffee/components/subsystem.h>

namespace h3 {

/*! The loaded Halo 3 map, held by its loader for as long as it is the
 * world: what gameplay looks tags up in */
struct H3Map : compo::SubsystemBase
{
    using type = H3Map;

    blam::elegy::map_container const* map{nullptr};

    bool loaded() const
    {
        return map != nullptr;
    }

    /*! f(map), if one is loaded */
    template<typename F>
    void visit(F&& f) const
    {
        if(map)
            f(*map);
    }
};

} // namespace h3
