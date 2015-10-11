#ifndef COFFEE_THREADING
#define COFFEE_THREADING

#include "coffee_types.h"
#include <thread>
#include <functional>
#include <future>

namespace Coffee{
namespace CFunctional{

namespace CThreading
{

static void runIndependent(std::function<void()> function)
{
    std::thread m_thread(function);
    m_thread.detach();
}

template<typename ReturnType>
static std::future<ReturnType> runAsync(std::function<ReturnType()> function)
{
    return std::async(std::launch::async,function);
}

static std::future<void> runAsyncVoid(std::function<void()> function);

template<typename DataType>
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
