#pragma once


#include <peripherals/libc/types.h>
#include <peripherals/stl/functional_types.h>
#include <peripherals/stl/stlstring_ops.h>
#include <peripherals/stl/types.h>

#include "async_tasks.h"

#if  __cplusplus >= 201703L && C_HAS_INCLUDE(<execution>)
#include <execution>
#endif

#if defined(THREAD_PROFILING)
using JobProfiler = Coffee::Profiler;
#endif

namespace stl_types {
namespace threads {

template<typename Ctxt, typename IterType>
C_DEPRECATED_S(
    "Use ParallelForEach instead, it will use the C++17 functions later")
FORCEDINLINE Future<void> ParallelFor(
    Function<void(IterType, Ctxt)> kernel,
    IterType                       iterations,
    Ctxt                           context,
    IterType                       weight = 1)
{
    return {};
}

namespace detail {

template<
    typename Parameters,
    typename std::enable_if<Parameters::batch_size >= 2>::type* = nullptr>
Function<void(szptr)> get_worker(
    Mutex&                                              work_lock,
    Function<void(typename Parameters::storage_type)>&& pred,
    typename Parameters::iterator&                      container_it,
    typename Parameters::iterator&                      container_end)
{
    return [&](szptr worker_i) {
        CurrentThread::SetName(
            "ParallelForEach::runner " + str::convert::to_string(worker_i));

#if defined(THREAD_PROFILING)
        JobProfiler::PushContext("Queue");
#endif

        while(true)
        {
            /* Fetch work item, exit if there is no
             * more work to do */
#if defined(THREAD_PROFILING)
            JobProfiler::PushContext("Awaiting lock");
#endif
            work_lock.lock();
            if(container_it == container_end)
            {
                work_lock.unlock();
#if defined(THREAD_PROFILING)
                JobProfiler::PopContext();
#endif
                break;
            }
#if defined(THREAD_PROFILING)
            JobProfiler::PopContext();

            JobProfiler::PushContext("Taking work item");
#endif
            /* The work item is copied to the worker */
            auto q_it = container_it;
            for(C_UNUSED(auto _) : Range<>(Parameters::batch_size))
            {
                if(container_it == container_end)
                    break;

                ++container_it;
            }
            auto q_end = container_it;

            work_lock.unlock();
#if defined(THREAD_PROFILING)
            JobProfiler::PopContext();

            JobProfiler::PushContext("Running work item(s)");
#endif
            /* Run predicate on work item */
            for(; q_it != q_end; ++q_it)
                pred(*q_it);
#if defined(THREAD_PROFILING)
            JobProfiler::PopContext();
#endif

        }
#if defined(THREAD_PROFILING)
        JobProfiler::PopContext();
#endif
    };
}

template<
    typename Parameters,
    typename std::enable_if<Parameters::batch_size == 1>::type* = nullptr>
Function<void(szptr)> get_worker(
    Mutex&                                              work_lock,
    Function<void(typename Parameters::storage_type)>&& pred,
    typename Parameters::iterator&                      container_it,
    typename Parameters::iterator&                      container_end)
{
    return [&](szptr worker_i) {
        CurrentThread::SetName(
            "ParallelForEach::runner " + str::convert::to_string(worker_i));

        while(true)
        {
            /* Fetch work item, exit if there is no
             * more work to do */
            work_lock.lock();
            if(container_it == container_end)
            {
                work_lock.unlock();
                break;
            }

            /* The work item is copied to the worker */
            typename Parameters::storage_type item = *container_it;
            ++container_it;
            work_lock.unlock();

            /* Run predicate on work item */
            pred(item);
        }
    };
}

} // namespace detail

template<size_t BatchSize, class ContainerT>
struct parametric_parallel
{
    using container_type = typename remove_cvref<ContainerT>::type;
    using value_type     = typename container_type::value_type;
    using iterator       = typename container_type::iterator;
    using storage_type   = typename declmemtype(iterator::operator*);

    static constexpr size_t batch_size = BatchSize;
};

namespace Parallel {

template<
    typename ContainerT,
    typename Parameters = parametric_parallel<1, ContainerT>>
/*!
 * \brief Parallel for-each loop over container. On C++17 compilers
 *  this will use std::for_each with parallel execution.
 *
 * The custom implementation does not divide the work up-front,
 *  but rather lets each worker grab a new item as they are finished.
 * This is not suitable for light-weight predicates, as that would
 *  be bottle-necked by the mutex.
 *
 * \param container Any iterable container
 * \param pred Predicate function for items from said container
 */
FORCEDINLINE void ForEach(
    ContainerT&                                         container,
    Function<void(typename Parameters::storage_type)>&& pred,
    szptr                                               num_workers = 0)
{
#if __cplusplus >= 201703L && C_HAS_INCLUDE(<execution>)
    std::for_each(
        std::execution::par_unseq,
        std::begin(container),
        std::end(container),
        pred);
#else
#if defined(COFFEE_NO_THREADLIB)
    auto thread_count = 1UL;
#else
    auto thread_count = Thread::hardware_concurrency();
#endif

    if(num_workers > 0)
        thread_count = num_workers;

    Vector<Future<void>> tasks;
    Mutex                work_lock;
    auto                 container_it  = container.begin();
    auto                 container_end = container.end();

    tasks.reserve(thread_count);

    Function<void(szptr)> runner = detail::get_worker<Parameters>(
        work_lock, std::move(pred), container_it, container_end);

    for(const auto i : Range<>(thread_count))
    {
        tasks.push_back(RunAsync(runner, i));
    }

    for(auto& task : tasks)
        task.get();
#endif
}

template<
    typename ContainerT,
    typename Parameters = parametric_parallel<1, ContainerT>>
FORCEDINLINE void Consume(
    ContainerT&&                                        container,
    Function<void(typename Parameters::storage_type)>&& pred,
    szptr                                               num_workers = 0)
{
    typename Parameters::container_type container_store = std::move(container);
    ForEach<typename Parameters::container_type, Parameters>(
        container_store, std::move(pred), num_workers);
}
}

} // namespace threads
} // namespace stl_types
