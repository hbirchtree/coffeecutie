#ifndef COFFEE_THREADING
#define COFFEE_THREADING

#include "coffee/core/CTypes"

#include <thread>
#include <functional>
#include <future>

namespace Coffee{
namespace CFunctional{
namespace CThreading{

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
        //TODO: Implement hash of thread IDs
        return std::hash<std::thread::id>()(m_id);
    }
private:
    const std::thread::id m_id;
};

template<typename... Args>
/*!
 * \brief Launch a task to run independently. Used as fire-and-forget, never to return.
 * \param function Function to run
 */
inline C_FORCE_INLINE void RunIndependent(std::function<void()> function,
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
inline C_FORCE_INLINE std::future<ReturnType> RunAsync(
        std::function<ReturnType()> function, Args... a)
{
    return std::async(std::launch::async,function,a...);
}

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

template<typename InUnit,typename OutUnit>
/*!
 * \brief Basic data transform function. Will take one value as input and output a certain other value, may produce more or less data than input. Buffers must be sufficiently big, and sizes or not checked! This is the user's responsibility.
 * \param processor
 * \param input
 * \param output
 * \return
 */
inline C_FORCE_INLINE std::future<bool> TransformData(
        CFunctionSlot<szptr,InUnit const&,OutUnit&>* processor,
        const _cbasic_data_chunk<InUnit>* input,
        _cbasic_data_chunk<OutUnit>* output)
{
    auto workThread = [](
            CFunctionSlot<void,InUnit const&,OutUnit*>* processor,
            const _cbasic_data_chunk<InUnit>* input,
            _cbasic_data_chunk<OutUnit>* output)
    {
        szptr out_ptr = 0;
        for(szptr i=0;i<input->elements;i++)
        {
            out_ptr += processor->call(input[i],&output[out_ptr]);
        }
        return false;
    };
    return RunAsync(workThread,processor,input,output);
}

typedef CThreadWorker<ubyte_t> CThreadUbyteWorker;

}
}

using ThreadId = CFunctional::CThreading::ThreadId_t;

}

#endif //COFFEE_THREADING
