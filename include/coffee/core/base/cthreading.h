#ifndef COFFEE_THREADING
#define COFFEE_THREADING

#include <thread>
#include <functional>
#include <future>

#include "../types/basetypes.h"
#include "../coffee_macros.h"
#include "../plat/environment/sysinfo_def.h"

namespace Coffee{
namespace CFunctional{
namespace Threads{

struct ThreadId_t
{
    ThreadId_t():
        m_id(std::this_thread::get_id())
    {
    }
    bool operator==(const ThreadId_t& thd) const
    {
        return m_id == thd.m_id;
    }
    uint64 hash()
    {
        return std::hash<std::thread::id>()(m_id);
    }
private:
    const std::thread::id m_id;
};

template<typename DataType>
/*!
 * \brief Contains a reference to an atomic pointer to be used with async worker
 */
struct CThreadWorker
{
    CThreadWorker(std::atomic<DataType>& atomic_ptr){
        thread_ptr = &atomic_ptr;
    }
    template<typename RType>
    std::future<RType> run(std::function<RType()> fun){
        return std::async(std::launch::async,fun);
    }
    std::future<void> run(std::function<void()> fun){
        return std::async(std::launch::async,fun);
    }
    std::atomic<DataType>* dataPtr() const{
        return thread_ptr;
    }
private:
    std::atomic<DataType>* thread_ptr = nullptr;
};

template<typename T>
/*!
 * \brief Check if a future is available. This is provided as a shortcut.
 * \param f A future to be checked
 * \return True if the value is available
 */
FORCEDINLINE bool FutureAvailable(std::future<T> const& f)
{
    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

template<typename... Args>
/*!
 * \brief Launch a task to run independently. Used as fire-and-forget, never to return.
 * \param function Function to run
 */
FORCEDINLINE void RunIndependent(std::function<void()> function,
                                          Args... a)
{
    std::thread m_thread(function,a...);
    m_thread.detach();
}

template<typename ReturnType,
         typename... Args>
/*!
 * \brief Launch a task asynchronously, returning a future value
 * \param function
 * \return A future object for return value
 */
FORCEDINLINE std::future<ReturnType> RunAsync(
	std::function<ReturnType(Args...)> function, Args... a)
{
    return std::async(std::launch::async,function,a...);
}

template<typename InUnit,typename OutUnit>
/*!
 * \brief Basic data transform function. Will take one value as input and output a certain other value, may produce more or less data than input. Buffers must be sufficiently big, sizes are not checked! This is the user's responsibility.
 * \param processor
 * \param input
 * \param output
 * \return
 */
FORCEDINLINE std::future<bool> TransformData(
        CFunctionSlot<szptr,InUnit const&,OutUnit&>* processor,
        const _cbasic_data_chunk<InUnit>* input,
        _cbasic_data_chunk<OutUnit>* output)
{
    auto workThread = [processor,input,output]()
    {
        szptr out_ptr = 0;
        for(szptr i=0;i<input->elements;i++)
        {
	    CASSERT(out_ptr<output->elements);
            out_ptr += processor->call(input[i],&output[out_ptr]);
        }
        return false;
    };
    return RunAsync(workThread,processor,input,output);
}

template<typename Ctxt,typename IterType>
/*!
 * \brief A parallelized for-loop, which can be fed with a data set (context) and kernel to do processing. It is recommended to process blocks of data instead of single elements, as this avoids a lot of function overhead. This function works at its peak when compiled with "-O3".
 * \param kernel Run on each iteration
 * \param iterations Number of iterations in total
 * \param context
 * \return
 */
FORCEDINLINE std::future<void> ParallelFor(
        std::function<void(IterType,Ctxt)> kernel,
        IterType iterations,
        Ctxt context,
        IterType weight = 1)
{
    std::function<void()> worker = [kernel,context,iterations,weight]()
    {
        uint32 thrcount = SysInfo::SmartParallelism(iterations,weight);

        std::vector<std::future<void>> tasks;

        IterType split = iterations/thrcount;
        IterType rest = iterations&thrcount;

        std::function<void(std::function<void(IterType,Ctxt)>,Ctxt,IterType)> fun = [](
                std::function<void(IterType,Ctxt)> kern,
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

typedef CThreadWorker<byte_t> CThreadByteWorker;

}
}

using ThreadId = CFunctional::Threads::ThreadId_t;

}

#endif //COFFEE_THREADING
