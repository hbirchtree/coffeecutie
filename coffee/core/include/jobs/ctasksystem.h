#ifndef COFFEE_TASK_SYSTEM_H
#define COFFEE_TASK_SYSTEM_H

#include <atomic>
#include <queue>
#include <mutex>

namespace Coffee{
namespace CFunctional{
namespace CThreading{

struct CTask;

using CTaskFunction = void(*)(CTask*,const void*);
using CTaskCounter = std::atomic_int_least64_t;

struct CTask
{
    CTaskFunction task_function;
    CTask* parent;
    CTaskCounter unfinished_tasks;
};

template<typename T> struct _cbasic_multiqueue
{
public:
    void add(T item)
    {
        mutex.lock();
        queue.push(item);
        mutex.unlock();
    }
    bool empty()
    {
        mutex.lock();
        bool status = queue.empty();
        mutex.unlock();
        return status;
    }
    T pop()
    {
        mutex.lock();
        T item = queue.front();
        queue.pop();
        mutex.unlock();
        return item;
    }

private:
    std::mutex mutex;
    std::queue<T> queue;
};

typedef _cbasic_multiqueue<CTask*> CTaskQueue;

struct CTaskContext
{
    CTaskQueue tasks_delete_queue;
};

extern CTask* coffee_jobs_create_task(
        CTaskFunction fun,
        CTask *parent = nullptr);

extern void coffee_jobs_finish(CTaskContext *ctxt,
        CTask* task);

extern void coffee_jobs_garbage_collect(CTaskContext *ctxt);

}
}
}

#endif
