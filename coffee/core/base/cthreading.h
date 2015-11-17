#ifndef COFFEE_THREADING
#define COFFEE_THREADING

#include "coffee/core/CTypes"

#include <thread>
#include <functional>
#include <future>

namespace Coffee{
namespace CFunctional{

namespace CThreading
{

/*!
 * \brief Launch a task to run independently. Used as fire-and-forget, never to return.
 * \param function Function to run
 */
inline static void runIndependent(std::function<void()> function)
{
    std::thread m_thread(function);
    m_thread.detach();
}

template<typename ReturnType>
/*!
 * \brief Launch a task asynchronously, returning a future value
 * \param function
 * \return A future object for return value
 */
static std::future<ReturnType> runAsync(std::function<ReturnType()> function)
{
    return std::async(std::launch::async,function);
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

typedef CThreadWorker<ubyte> CThreadUbyteWorker;

}
}
}

#endif //COFFEE_THREADING
