#include <coffee/core/CDebug>
#include <coffee/core/CProfiling>
#include <coffee/core/task_queue/task.h>

#define RQ_API "RuntimeQueue::"

namespace Coffee {

RuntimeQueue::QueueContextPtr RuntimeQueue::context;

RuntimeQueue* RuntimeQueue::CreateNewQueue(const CString& name)
{
    Lock _(context->globalMod);

    auto t_id = ThreadId().hash();

    auto q_it = context->queues.find(t_id);

    if(q_it == context->queues.end())
    {
        context->queues.insert({t_id, RuntimeQueue()});
        Profiler::LabelThread(name.c_str());
        return &context->queues[t_id];
    } else
        return &(*q_it).second;
}

static void ImpCreateNewThreadQueue(CString const& name, AtomicBool** flag)
{
#ifndef COFFEE_LOWFAT
    try
    {
#endif
        /* Enable profiler and etc. */
        State::SetInternalThreadState(State::CreateNewThreadState());

        AtomicBool runtimeFlag;
        runtimeFlag.store(true);

        RuntimeQueue* queue = RuntimeQueue::CreateNewQueue(name);

        *flag = &runtimeFlag;

        Profiler::DeepPushContext(RQ_API "Running queue");

        while(runtimeFlag.load())
        {
            queue->executeTasks();

            Profiler::DeepPushContext("Sleeping");
            auto sleepTime =
                queue->timeTillNext(std::chrono::milliseconds(500));
            CurrentThread::sleep_for(sleepTime);
            Profiler::DeepPopContext();
        }

        cVerbose(8, "Shutting down thread");

        Profiler::DeepPopContext();

#ifndef COFFEE_LOWFAT
    } catch(std::exception const& e)
    {
        cWarning(
            RQ_API "Uncaught exception!\n"
                   "\n"
                   "\t{0}: {1}"
                   "\n",
            e.what(),
            Stacktracer::DemangleSymbol(typeid(e).name()));
    }
#endif
}

RuntimeQueue* RuntimeQueue::CreateNewThreadQueue(const CString& name)
{
    try{
    DProfContext _(DTEXT(RQ_API "Creating new task queue"));
    AtomicBool*  flagPtr = nullptr;
    Thread       t(ImpCreateNewThreadQueue, name, &flagPtr);
    auto         tid = ThreadId(t.get_id()).hash();

    CurrentThread::yield();

    /* I feel bad for this, but it shouldn't be called often */
    while(flagPtr == nullptr)
    {
        CurrentThread::yield();
    }

    {
        Lock _(context->globalMod);

        context->queueThreads[tid] = std::move(t);
        context->queueFlags.insert({tid, flagPtr});
    }

    return &context->queues.find(tid)->second;
    } catch(std::exception const& e)
    {
        cWarning("Failed to create RuntimeQueue: {0}", e.what());
        return nullptr;
    }
}

RuntimeQueue* RuntimeQueue::GetCurrentQueue()
{
    Lock _(context->globalMod);

    auto q_id = ThreadId().hash();
    auto q_it = context->queues.find(q_id);

    if(q_it != context->queues.end())
        return &(*q_it).second;
    else
        return nullptr;
}

RuntimeTask* RuntimeQueue::GetSelf()
{
    RuntimeQueue* q = GetCurrentQueue();

    if(!q)
        return nullptr;

    szptr idx = 0;

    for(auto i : Range<>(q->mTasks.size()))
        if(q->mTasks[i].index == q->mCurrentTaskId)
        {
            idx = i;
            break;
        }

    /* To avoid casting the from size_t to signed,
     *  check if the validation holds */
    if(idx != 0 || q->mTasks[0].index != q->mCurrentTaskId)
        return nullptr;

    return &q->mTasks[idx].task;
}

u64 RuntimeQueue::GetSelfId()
{
    RuntimeQueue* q = GetCurrentQueue();

    if(!q)
        return 0;

    return q->mCurrentTaskId;
}

static bool VerifyTask(RuntimeTask const& t)
{
    if(!(t.flags & (RuntimeTask::SingleShot | RuntimeTask::Periodic)))
        return false;
    if((t.flags & RuntimeTask::SingleShot) && (t.flags & RuntimeTask::Periodic))
        return false;
    if((t.flags & RuntimeTask::SingleShot) &&
       !(t.flags & RuntimeTask::Immediate) &&
       t.time < RuntimeTask::clock::now())
        return false;
    return true;
}

u64 RuntimeQueue::Queue(RuntimeTask&& task)
{
    return Queue(ThreadId(), std::move(task));
}

u64 RuntimeQueue::Queue(ThreadId const& targetThread, RuntimeTask&& task)
{
    if(!VerifyTask(task))
        return 0;

    if(task.flags & RuntimeTask::Periodic)
    {
        task.time = RuntimeTask::clock::now() + task.interval;
    }

    Lock _(context->globalMod);

    auto q_it = context->queues.find(targetThread.hash());

    if(q_it == context->queues.end())
        return 0;
    else
    {
        auto& ref = (*q_it).second;
        Profiler::DeepProfile(RQ_API "Adding task to Queue");
        return ref.enqueue(std::move(task));
    }
}

/*!
 * \brief We can look, but we cannot touch. This is used to
 *  inspect RuntimeTasks as an observer. Only the controlling
 *  thread may actually modify them.
 * \param ids
 * \param tasks
 * \param taskId
 * \return
 */
static RuntimeTask const* GetTask(
    Vector<RuntimeQueue::task_data_t> const& tasks,
    u64                                      taskId,
    szptr*                                   idx = nullptr)
{
    /* Locate the task in our vector */
    szptr i;
    bool  found = false;
    for(i = 0; i < tasks.size(); i++)
    {
        if(tasks[i].index == taskId)
        {
            found = true;
            break;
        }
    }

    if(!found)
        return nullptr;

    if(idx)
        *idx = i;

    return &tasks[i].task;
}

bool RuntimeQueue::Block(u64 taskId)
{
    return Block(ThreadId(), taskId);
}

bool RuntimeQueue::Block(const ThreadId& targetThread, u64 taskId)
{
    Lock _(context->globalMod);

    auto q_it = context->queues.find(targetThread.hash());

    if(q_it == context->queues.end())
        return false;

    auto& queue = (*q_it).second;

    RuntimeTask const* task = nullptr;
    szptr              idx  = 0;

    if(!(task = GetTask(queue.mTasks, taskId, &idx)))
        return false;

    /* We do this check in case we are executing in the queue */
    /* Otherwise we deadlock */
    if(queue.mCurrentTaskId == 0)
        queue.mTasksLock.lock();

    if(!queue.mTasks[idx].alive)
        return false;

    Profiler::DeepProfile(RQ_API "Blocking task");
    queue.mTasks[idx].alive = false;

    queue.mTasksLock.unlock();

    return true;
}

bool RuntimeQueue::Unblock(u64 taskId)
{
    return Unblock(ThreadId(), taskId);
}

bool RuntimeQueue::Unblock(const ThreadId& targetThread, u64 taskId)
{
    Lock _(context->globalMod);

    auto q_it = context->queues.find(targetThread.hash());

    if(q_it == context->queues.end())
        return false;

    auto& queue = (*q_it).second;

    RuntimeTask const* task = nullptr;
    szptr              idx  = 0;

    if(!(task = GetTask(queue.mTasks, taskId, &idx)))
        return false;

    /* We do this check in case we are executing in the queue */
    /* Otherwise we deadlock */
    if(queue.mCurrentTaskId == 0)
        queue.mTasksLock.lock();

    if(queue.mTasks[idx].alive)
        return false;

    Profiler::DeepProfile(RQ_API "Unblocking task");
    queue.mTasks[idx].alive = true;

    queue.mTasksLock.unlock();

    return true;
}

bool RuntimeQueue::CancelTask(u64 taskId)
{
    return CancelTask(ThreadId(), taskId);
}

bool RuntimeQueue::CancelTask(const ThreadId& targetThread, u64 taskId)
{
    Lock _(context->globalMod);

    auto q_it = context->queues.find(targetThread.hash());

    if(q_it == context->queues.end())
        return false;

    auto& queue = (*q_it).second;

    RuntimeTask const* task = nullptr;
    szptr              idx  = 0;

    if(!(task = GetTask(queue.mTasks, taskId, &idx)))
        return false;

    /* We do this check in case we are executing in the queue */
    /* Otherwise we deadlock */
    if(queue.mCurrentTaskId == 0)
        queue.mTasksLock.lock();

    queue.mTasks[idx].alive      = false;
    queue.mTasks[idx].to_dispose = true;

    queue.mTasksLock.unlock();

    return true;
}

void RuntimeQueue::AwaitTask(u64 taskId)
{
    AwaitTask(ThreadId(), taskId);
}

void RuntimeQueue::AwaitTask(const ThreadId& targetThread, u64 taskId)
{
    auto queue = context->queues.find(targetThread.hash());

    /* If thread has no queue, return */
    if(queue == context->queues.end())
        return;

    auto const& queueRef = queue->second;

    RuntimeTask const* task = nullptr;
    szptr              idx  = 0;

    if(!(task = GetTask(queueRef.mTasks, taskId, &idx)))
        return;

    /* We cannot reliably await periodic tasks */
    if(task->flags & RuntimeTask::Periodic)
        return;

    /* Do not await a past event */
    if(!(task->flags & RuntimeTask::Immediate) &&
       RuntimeTask::clock::now() > task->time)
        return;

    CurrentThread::sleep_until(task->time);

    /* I know this is bad, but we must await the task */
    while(queueRef.mTasks[idx].alive)
        CurrentThread::yield();
}

void RuntimeQueue::TerminateThread(RuntimeQueue* thread)
{
    if(!thread)
        return;

    auto tid = thread->threadId().hash();

    context->queueFlags[tid]->store(false);
    context->queueThreads[tid].join();

    context->queueFlags.erase(tid);
    context->queueThreads.erase(tid);
}

void RuntimeQueue::TerminateThreads()
{
    Lock _(context->globalMod);

    for(auto t : context->queueFlags)
    {
        t.second->store(false);
    }

    for(auto& t : context->queueThreads)
    {
        t.second.join();
    }
}

RuntimeQueue::QueueContextPtr RuntimeQueue::CreateContext()
{
    return MkShared<QueueContext>();
}

bool RuntimeQueue::SetQueueContext(RuntimeQueue::QueueContextPtr i)
{
    if(context)
        return false;

    context = i;
    return true;
}

RuntimeQueue::QueueContextPtr RuntimeQueue::GetQueueContext()
{
    return context;
}

void RuntimeQueue::executeTasks()
{
    DProfContext __(DTEXT(RQ_API "Executing thread tasks"));

    Profiler::DeepPushContext(RQ_API "Locking queue");
    Lock _(mTasksLock);
    Profiler::DeepPopContext();

    auto currTime = RuntimeTask::clock::now();

    szptr i = 0;
    for(task_data_t& task : mTasks)
    {
        /* If this task has to be run in the future,
         *  all proceeding tasks will do as well */
        if(task.task.time > currTime)
        {
            i++;
            break;
        }

        /* Skip dead tasks, clean it up later */
        if(!mTasks[i].alive)
        {
            i++;
            continue;
        }

        /* In this case we will let it run */
        mCurrentTaskId = mTasks[i].index;
        Profiler::DeepPushContext(RQ_API "Running task");
        task.task.task();
        Profiler::DeepPopContext();
        mCurrentTaskId = 0;

        /* Now, if a task is single-shot, remove it */
        if(task.task.flags & RuntimeTask::SingleShot)
            mTasks[i].alive = false;
        else if(task.task.flags & RuntimeTask::Periodic)
        {
            task.task.time = RuntimeTask::clock::now() + task.task.interval;
        } else
        {
            Throw(implementation_error("unknown task type"));
        }
        i++;
    }
}

RuntimeTask::Duration RuntimeQueue::timeTillNext(RuntimeTask::Duration fallback)
{
    Lock _(mTasksLock);

    RuntimeTask::Timepoint current = RuntimeTask::clock::now();

    if(mTasks.size() == 0)
        return fallback;
    else
    {
        if(mTasks[0].task.time < current)
            return std::chrono::milliseconds(0);
        else
            return mTasks[0].task.time - current;
    }
}

CString RuntimeQueue::name()
{
    Lock _(mTasksLock);
    return {};
}

ThreadId RuntimeQueue::threadId()
{
    return mThreadId;
}

RuntimeQueue::RuntimeQueue(RuntimeQueue const& queue)
{
    mTasks     = std::move(queue.mTasks);
    mTaskIndex = std::move(queue.mTaskIndex);
}

RuntimeQueue::RuntimeQueue() :
    mTasks(), mTasksLock(), mTaskIndex(0), mCurrentTaskId(0)
{
}

u64 RuntimeQueue::enqueue(RuntimeTask&& task)
{
    Lock _(mTasksLock);

    u64 output = ++mTaskIndex;

    task_data_t task_d;
    task_d.task       = std::move(task);
    task_d.index      = output;
    task_d.alive      = true;
    task_d.to_dispose = false;

    mTasks.push_back(task_d);

    sortTasks();

    return output;
}

void RuntimeQueue::sortTasks()
{
    std::sort(mTasks.begin(), mTasks.end());
}

} // namespace Coffee