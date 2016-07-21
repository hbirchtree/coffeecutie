#pragma once

#include "async_tasks.h"
#include "../../plat/plat_environment.h"
#include "../../types/tdef/integertypes.h"
#include "../../types/tdef/stltypes.h"

namespace Coffee{
namespace Threads{

template<typename Ctxt,typename IterType>
/*!
 * \brief A parallelized for-loop, which can be fed with a data set (context) and kernel to do processing. It is recommended to process blocks of data instead of single elements, as this avoids a lot of function overhead. This function works at its peak when compiled with "-O3".
 * \param kernel Run on each iteration
 * \param iterations Number of iterations in total
 * \param context
 * \param weight A specific weight given to each job, used to determine how many tasks can be run in parallel on each core
 * \return
 */
FORCEDINLINE Future<void> ParallelFor(
        Function<void(IterType,Ctxt)> kernel,
        IterType iterations,
        Ctxt context,
        IterType weight = 1)
{
    Function<void()> worker = [kernel,context,iterations,weight]()
    {
        uint32 thrcount = SysInfo::SmartParallelism(iterations,weight);

        Vector<Future<void>> tasks;

        IterType split = iterations/thrcount;
        IterType rest = iterations&thrcount;

        Function<void(Function<void(IterType,Ctxt)>,Ctxt,IterType)> fun = [](
                Function<void(IterType,Ctxt)> kern,
                Ctxt context,
                IterType count)
        {
            for(IterType a=0;a<count;a++)
            {
                kern(a,context);
            }
        };

        for(uint32 i=0;i<thrcount;i++)
        {
            IterType itc = split;
            if(i==0)
                itc+=rest;
            tasks.push_back(RunAsync<void>(fun,kernel,context,split));
        }

        for(uint32 i=0;i<thrcount;i++)
            tasks[i].get();

        return;
    };
    return RunAsync(worker);
}

}
}
