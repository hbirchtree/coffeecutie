#pragma once

#include "../../plat/plat_sysinfo.h"
#include "../../types/tdef/integertypes.h"
#include "../../types/tdef/stltypes.h"
#include "async_tasks.h"

namespace Coffee {
namespace Threads {

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
FORCEDINLINE Future<void> ParallelFor(
    Function<void(IterType, Ctxt)> kernel,
    IterType                       iterations,
    Ctxt                           context,
    IterType                       weight = 1)
{
    Function<void()> worker = [kernel, context, iterations, weight]() {
        u32 thrcount = SysInfo::SmartParallelism(iterations, weight);

        Vector<Future<void>> tasks;

        IterType split = iterations / thrcount;
        IterType rest  = iterations & thrcount;

        Function<void(Function<void(IterType, Ctxt)>, Ctxt, IterType)> fun =
            [](Function<void(IterType, Ctxt)> kern,
               Ctxt                           context,
               IterType                       count) {
                for(IterType a = 0; a < count; a++)
                {
                    kern(a, context);
                }
            };

        for(u32 i = 0; i < thrcount; i++)
        {
            IterType itc = split;
            if(i == 0)
                itc += rest;
            tasks.push_back(RunAsync(fun, kernel, context, split));
        }

        for(u32 i = 0; i < thrcount; i++)
            tasks[i].get();

        return;
    };
    return RunAsync(worker);
}

template<typename ContainerT, typename T>
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
    auto thread_count = std::thread::hardware_concurrency();
#endif

    if(num_workers > 0)
        thread_count = num_workers;

    Vector<Future<void>> tasks;
    Mutex                work_lock;
    auto                 container_it  = container.begin();
    auto                 container_end = container.end();

    tasks.reserve(thread_count);

    Function<void()> runner = [&]() {
        while(true)
        {
            /* Fetch work item, exit if there is no more work to do */
            work_lock.lock();
            if(container_it == container_end)
            {
                work_lock.unlock();
                break;
            }

            /* The work item is copied to the worker */
            T& item = *container_it;
            container_it++;
            work_lock.unlock();

            /* Run predicate on work item */
            pred(item);
        }
    };

    for(const auto i : Range<>(thread_count))
    {
        C_UNUSED(i);
        tasks.push_back(RunAsync(runner));
    }

    for(auto& task : tasks)
        task.get();
#else
    throw implementation_error("C++17 code for this not written yet!");
#endif
}

} // namespace Threads
} // namespace Coffee
