#ifndef COFFEE_FUNCTIONAL_H
#define COFFEE_FUNCTIONAL_H

#include <thread>
#include <functional>
#include <future>

namespace Coffee{
namespace CFunctional{

template<typename RType, typename... AType>
class CRFunction {
public:
    virtual RType exec(AType... args) = 0;
};

template<class TClass, class FClass, typename RType, typename... AType>
class CRMemberFunction : public CRFunction<RType,AType...>{
    typedef RType (FClass::*FuncPtr)(AType...);

    TClass* m_instance;
    FuncPtr m_func;
public:
    CRMemberFunction(TClass* obj, RType (FClass::*fun)(AType...)){
        m_instance = obj;
        m_func = fun;
    }
    RType exec(AType... args){
        return (((FClass*)m_instance)->*m_func)(args...);
    }
};

template<typename RType,typename... AType>
class CRStaticFunction : public CRFunction<RType,AType...>{
    typedef RType (*FuncPtr)(AType...);

    FuncPtr m_func;
public:
    CRStaticFunction(FuncPtr fun){
        m_func = fun;
    }
    RType exec(AType... args){
        return (*m_func)(args...);
    }
};
namespace CThreading
{
typedef std::thread::id thread_id;

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
    std::atomic<DataType>* dataPtr() const{
        return thread_ptr;
    }
private:
    std::atomic<DataType>* thread_ptr = nullptr;
};

}
}
}


#endif // COFFEE_FUNCTIONAL_H
