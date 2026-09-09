#pragma once

namespace stl_types {

template<typename T, typename F, typename C>
T accumulate(C const& container, T init, F binary_op)
{
    T out = init;
    for(auto const& v : container)
        out = binary_op(v, out);
    return out;
}

}
