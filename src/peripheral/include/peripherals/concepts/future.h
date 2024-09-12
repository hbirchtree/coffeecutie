#pragma once

#include <chrono>

namespace stl_types {

template<typename T>
concept is_future_t = requires(T& fut) {
    {
        fut.get()
    };
    {
        fut.wait_for(std::chrono::milliseconds())
    };
};

}
