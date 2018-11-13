#pragma once

#include "async_tasks.h"
#include <peripherals/libc/types.h>
#include <peripherals/stl/functional_types.h>
#include <peripherals/stl/stlstring_ops.h>
#include <peripherals/stl/types.h>

namespace stl_types {
namespace threads {

template<typename Ctxt, typename IterType>
/*!
 * \brief A parallelized for-loop, which can be fed with a data set (context)
 * and kernel to do processing. It is recommended to process blocks of data
 * instead of single elements, as this avoids a lot of function overhead. This
 * function works at its peak when compiled with "-O3". \param kernel Run on
 * each iteration \param iterations Number of iterations in total \param context
 * \param weight A specific weight given to each job, used to determine how many
 * tasks can be run in parallel on each core \return
 */
C_DEPRECATED_S(
    "Use ParallelForEach instead, it will use the C++17 functions later")
    FORCEDINLINE Future<void> ParallelFor(
        Function<void(IterType, Ctxt)> kernel,
        IterType                       iterations,
        Ctxt                           context,
        IterType                       weight = 1)
{
}

namespace detail {

template<
    typename ContainerT,
    typename T,
    typename IteratorT,
    typename StorageType =
        typename decltype(std::mem_fun(&IteratorT::operator*))::result_type>
Function<void(szptr)> get_worker(
    Mutex&               work_lock,
    Function<void(T&)>&& pred,
    IteratorT&           container_it,
    IteratorT&           container_end)
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

            /* The work item is copied to the worker
             */
            StorageType item = *container_it;
            ++container_it;
            work_lock.unlock();

            /* Run predicate on work item */
            pred(item);
        }
    };
}

} // namespace detail

template<
    typename ContainerT,
    typename T         = typename ContainerT::value_type,
    typename IteratorT = typename ContainerT::iterator>
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
FORCEDINLINE void ParallelForEach(
    ContainerT& container, Function<void(T&)>&& pred, u32 num_workers = 0)
{
#if __cplusplus < 201703L
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

    Function<void(szptr)> runner = detail::get_worker<ContainerT, T, IteratorT>(
        work_lock, std::move(pred), container_it, container_end);

    for(const auto i : Range<>(thread_count))
    {
        tasks.push_back(RunAsync(runner, i));
    }

    for(auto& task : tasks)
        task.get();
#else
    throw implementation_error("C++17 code for this not written yet!");
#endif
}

template<typename ContainerT, typename T>
FORCEDINLINE void ParallelForEach(
    ContainerT&& container, Function<void(T&)>&& pred, u32 num_workers = 0)
{
    ParallelForEach(container, std::move(pred), num_workers);
}

} // namespace Threads
} // namespace stl_types
