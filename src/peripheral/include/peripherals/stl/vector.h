#pragma once

#include <vector>

namespace stl_types {

template<typename T, typename Pred>
typename std::vector<T>::size_type erase_if(std::vector<T>& container, Pred pred)
{
#if defined(__cpp_lib_algorithm_default_value_type) && __cpp_lib_algorithm_default_value_type > 202404u
    return std::erase_if(container, pred);
#else
    auto it = std::remove_if(std::begin(container), std::end(container), pred);
    auto r = std::end(container) - it;
    container.erase(it, std::end(container));
    return r;
#endif
}

}
