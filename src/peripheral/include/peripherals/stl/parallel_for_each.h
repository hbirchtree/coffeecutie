#pragma once

#include <peripherals/identify/compiler/function_inlining.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

#include <algorithm>
#include <thread>

namespace stl_types {
namespace detail {

template<typename Fun, typename It>
void parallel_for_each_predicate(It const& start, It const& end, Fun& predicate)
{
    std::for_each(start, end, predicate);
}

} // namespace detail

template<typename Fun, typename It>
STATICINLINE void parallel_for_each(
    It const& start, It const& end, Fun&& predicate, libc_types::u32 workers)
{
    auto num_items = end - start;
    if(num_items <= 0)
        return;
    if(num_items < workers && num_items > 1)
        workers = num_items;
    auto per_worker = num_items / workers;

    std::list<std::thread> threads;
    for(auto i : stl_types::Range<>(std::max(workers - 1, 1u)))
    {
        threads.emplace_back(
            detail::parallel_for_each_predicate<
                decltype(predicate),
                decltype(start)>,
            start + per_worker * i,
            start + per_worker * (i + 1),
            std::ref(predicate));
    }
    detail::parallel_for_each_predicate(
        start + per_worker * (workers - 1), end, predicate);

    for(auto& thread : threads)
        thread.join();
}

template<typename Fun, typename C>
STATICINLINE void parallel_for_each(
    C&& container, Fun&& predicate, libc_types::u32 workers)
{
    return parallel_for_each(
        container.begin(), container.end(), std::move(predicate), workers);
}

} // namespace stl_types
