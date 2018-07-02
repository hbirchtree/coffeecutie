#include <coffee/core/plat/plat_stacktrace.h>
#include <coffee/core/task_queue/task.h>

#include <coffee/core/CProfiling>

#include <coffee/core/CDebug>

#define RQ_API "RuntimeQueue::"

namespace Coffee {

using RQE = RuntimeQueueError;

RuntimeQueue::QueueContextPtr RuntimeQueue::context;

const char* runtime_queue_category::name() const noexcept
{
    return "RuntimeQueue";
}

std::string runtime_queue_category::message(int error_code) const
{
    switch(C_CAST<RQE>(error_code))
    {
    case RQE::ExpiredTaskDeadline:
        return "RuntimeTask's deadline is already expired";
    case RQE::IncompatibleTaskAwait:
        return "Provided RuntimeTask cannot be awaited";
    case RQE::InvalidQueue:
        return "Invalid reference to RuntimeQueue";
    case RQE::InvalidTaskFlags:
        return "Invalid combination of RuntimeTask flags";
    case RQE::InvalidTaskId:
        return "Invalid reference to RuntimeTask";
    case RQE::InvalidTaskLifetime:
        return "RuntimeTask cannot be awaited";
    case RQE::TaskAlreadyBlocked:
        return "Attempt to block already blocked task";
    case RQE::TaskAlreadyStarted:
        return "Attempt to unblock already running task";
    case RQE::ThreadSpawn:
        return "Failed to spawn dedicated task thread";
    case RQE::UncaughtException:
        return "Thread died from uncaught exception";
    }

	throw implementation_error("unimplemented error message");
}

static bool VerifyTask(RuntimeTask const& t, runtime_queue_error& ec)
{
    if(!(t.flags & (RuntimeTask::SingleShot | RuntimeTask::Periodic)))
    {
        ec = RQE::InvalidTaskFlags;
        return false;
    }

    if((t.flags & RuntimeTask::SingleShot) && (t.flags & RuntimeTask::Periodic))
    {
        ec = RQE::InvalidTaskFlags;
        return false;
    }

    if((t.flags & RuntimeTask::SingleShot) &&
       !(t.flags & RuntimeTask::Immediate) &&
       t.time < RuntimeTask::clock::now())
    {
        ec = RQE::ExpiredTaskDeadline;
        return false;
    }
    return true;
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
    runtime_queue_error&                     ec,
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
    {
        ec = RQE::InvalidTaskId;
        return nullptr;
    }

    if(idx)
        *idx = i;

    return &tasks[i].task;
}

static void NotifyThread(
    ShPtr<RuntimeQueue::QueueContext> context,
    ThreadId::Hash                    threadId,
    RuntimeTask::Duration             previousDeadline,
    RuntimeTask::Timepoint            currentBase)
{
    C_PTR_CHECK(context);

    auto threadFlags = context->queueFlags.find(threadId);
    auto queueRef    = context->queues.find(threadId);

    if(threadFlags == context->queueFlags.end() ||
       queueRef == context->queues.end())
        return;

    C_PTR_CHECK(threadFlags->second);

    /* We want to notify when:
     * - A task is scheduled to run sooner than the thread will wake up
     * - A task that will run is cancelled, and sleep can be rescheduled
     *
     * In essence, if our new event comes first in the timeline,
     *  we need to reschedule the worker.
     */

    auto& condition = threadFlags->second->condition;
    auto& queue     = queueRef->second;

    auto wakeupTime = queue.timeTillNext(currentBase);

    if(wakeupTime < previousDeadline)
    {
        Profiler::DeepProfile(RQ_API "Notifying thread");
        condition.notify_one();
    }
}

RuntimeQueue* RuntimeQueue::CreateNewQueue(const CString& name)
{
    C_PTR_CHECK(context);

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

static void ImpCreateNewThreadQueue(
    CString const& name, ShPtr<RuntimeQueue::semaphore_t>* sem)
{
    runtime_queue_error ec;

    ShPtr<RuntimeQueue::semaphore_t> sem_ = *sem;

#ifndef COFFEE_LOWFAT
    try
    {
#endif
        /* Enable profiler and etc. */
        State::SetInternalThreadState(State::CreateNewThreadState());

        /* First create the queue object... */
        RuntimeQueue* queue = RuntimeQueue::CreateNewQueue(name);

        /* Then notify our parent that everything is done */
        sem_->condition.notify_one();

        /* We use a mutex to allow our parent to notify us of work
         *  or changes in the queue, to allow rescheduling. */
        UqLock thread_lock(sem_->mutex);

        Profiler::DeepPushContext(RQ_API "Running queue");
        while(sem_->running.load())
        {
            queue->executeTasks();

            Profiler::DeepPushContext("Sleeping");
            auto sleepTime = queue->timeTillNext();
            sem_->condition.wait_for(thread_lock, sleepTime);
            Profiler::DeepPopContext();
        }

        Profiler::DeepPopContext();

#ifndef COFFEE_LOWFAT
    } catch(std::exception const& e)
    {
        ec = RQE::UncaughtException;
        ec = Stacktracer::DemangleSymbol(typeid(e).name()) + ": " + e.what();
    }
#endif

    if(ec)
        cWarning(
            RQ_API "Uncaught exception!\n"
                   "\n"
                   "\t{0}"
                   "\n",
            ec.message());
}

RuntimeQueue* RuntimeQueue::CreateNewThreadQueue(const CString& name, rqe& ec)
{
    C_PTR_CHECK(context);

    try
    {
        DProfContext _(DTEXT(RQ_API "Creating new task queue"));

        ShPtr<semaphore_t> sem = MkShared<semaphore_t>();
        UqLock             temp_lock(sem->start_mutex);

        sem->running.store(true);

        /* Spawn the thread */
        Thread t(ImpCreateNewThreadQueue, name, &sem);
        auto   tid = ThreadId(t.get_id()).hash();

        /* Wait for the RuntimeQueue to be created on the thread */
        sem->condition.wait(temp_lock);

        {
            Lock _(context->globalMod);

            context->queueThreads[tid] = std::move(t);
            context->queueFlags.insert({tid, sem});
        }

        return &context->queues.find(tid)->second;
    } catch(std::exception const& e)
    {
        ec = RQE::ThreadSpawn;
        ec = e.what();
        return nullptr;
    }
}

RuntimeQueue* RuntimeQueue::GetCurrentQueue(rqe& ec)
{
    C_PTR_CHECK(context);

    Lock _(context->globalMod);

    auto q_id = ThreadId().hash();
    auto q_it = context->queues.find(q_id);

    if(q_it != context->queues.end())
        return &(*q_it).second;
    else
    {
        ec = RQE::InvalidQueue;
        return nullptr;
    }
}

RuntimeTask* RuntimeQueue::GetSelf(rqe& ec)
{
    RuntimeQueue* q = GetCurrentQueue(ec);

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
    {
        ec = RQE::InvalidTaskId;
        return nullptr;
    }

    return &q->mTasks[idx].task;
}

u64 RuntimeQueue::GetSelfId(rqe& ec)
{
    RuntimeQueue* q = GetCurrentQueue(ec);

    if(!q)
        return 0;

    return q->mCurrentTaskId;
}

u64 RuntimeQueue::Queue(RuntimeTask&& task, rqe& ec)
{
    return Queue(ThreadId(), std::move(task), ec);
}

u64 RuntimeQueue::Queue(
    ThreadId const& targetThread, RuntimeTask&& task, rqe& ec)
{
    if(!VerifyTask(task, ec))
        return 0;

    if(task.flags & RuntimeTask::Periodic)
    {
        task.time = RuntimeTask::clock::now() + task.interval;
    }

    Lock _(context->globalMod);

    auto thread_id = targetThread.hash();
    auto q_it      = context->queues.find(thread_id);

    if(q_it == context->queues.end())
    {
        ec = RQE::InvalidQueue;
        return 0;
    } else
    {
        auto& ref = (*q_it).second;
        Profiler::DeepProfile(RQ_API "Adding task to Queue");

        RecLock __(q_it->second.mTasksLock);

        auto currentBase      = RuntimeTask::clock::now();
        auto previousNextTime = ref.timeTillNext(currentBase);

        auto id = ref.enqueue(std::move(task));

        NotifyThread(context, thread_id, previousNextTime, currentBase);

        return id;
    }
}

bool RuntimeQueue::Block(const ThreadId& targetThread, u64 taskId, rqe& ec)
{
    Lock         _(context->globalMod);
    DProfContext __(RQ_API "Blocking task");

    auto thread_id = targetThread.hash();
    auto q_it      = context->queues.find(thread_id);

    if(q_it == context->queues.end())
    {
        ec = RQE::InvalidQueue;
        return false;
    }

    auto& queue = (*q_it).second;

    RuntimeTask const* task = nullptr;
    szptr              idx  = 0;

    if(!(task = GetTask(queue.mTasks, taskId, ec, &idx)))
    {
        ec = RQE::InvalidTaskId;
        return false;
    }

    /* We do this check in case we are executing in the queue */
    /* Otherwise we deadlock */
    RecLock ___(queue.mTasksLock);

    if(!queue.mTasks[idx].alive)
    {
        ec = RQE::TaskAlreadyBlocked;
        return false;
    }

    queue.mTasks[idx].alive = false;

    auto currentBase      = RuntimeTask::clock::now();
    auto previousNextTime = queue.timeTillNext(currentBase);

    NotifyThread(context, thread_id, previousNextTime, currentBase);


    return true;
}

bool RuntimeQueue::Unblock(const ThreadId& targetThread, u64 taskId, rqe& ec)
{
    Lock         _(context->globalMod);
    DProfContext __(RQ_API "Unblocking task");

    auto thread_id = targetThread.hash();
    auto q_it      = context->queues.find(thread_id);

    if(q_it == context->queues.end())
    {
        ec = RQE::InvalidQueue;
        return false;
    }

    auto& queue = (*q_it).second;

    RuntimeTask const* task = nullptr;
    szptr              idx  = 0;

    if(!(task = GetTask(queue.mTasks, taskId, ec, &idx)))
    {
        ec = RQE::InvalidTaskId;
        return false;
    }

    /* We do this check in case we are executing in the queue */
    /* Otherwise we deadlock */
    if(queue.mCurrentTaskId == 0)
        queue.mTasksLock.lock();

    if(queue.mTasks[idx].alive)
    {
        if(queue.mCurrentTaskId == 0)
            queue.mTasksLock.unlock();
        ec = RQE::TaskAlreadyStarted;
        return false;
    }

    queue.mTasks[idx].alive = true;

    auto currentBase      = RuntimeTask::clock::now();
    auto previousNextTime = queue.timeTillNext(currentBase);

    NotifyThread(context, thread_id, previousNextTime, currentBase);

    if(queue.mCurrentTaskId == 0)
        queue.mTasksLock.unlock();

    return true;
}

bool RuntimeQueue::CancelTask(const ThreadId& targetThread, u64 taskId, rqe& ec)
{
    Lock _(context->globalMod);

    auto thread_id = targetThread.hash();
    auto q_it      = context->queues.find(thread_id);

    if(q_it == context->queues.end())
    {
        ec = RQE::InvalidQueue;
        return false;
    }

    auto& queue = (*q_it).second;

    RuntimeTask const* task = nullptr;
    szptr              idx  = 0;

    if(!(task = GetTask(queue.mTasks, taskId, ec, &idx)))
    {
        ec = RQE::InvalidTaskId;
        return false;
    }

    /* We do this check in case we are executing in the queue */
    /* Otherwise we deadlock */
    if(queue.mCurrentTaskId == 0)
        queue.mTasksLock.lock();

    queue.mTasks[idx].alive      = false;
    queue.mTasks[idx].to_dispose = true;

    auto currentBase      = RuntimeTask::clock::now();
    auto previousNextTime = queue.timeTillNext(currentBase);

    NotifyThread(context, thread_id, previousNextTime, currentBase);

    if(queue.mCurrentTaskId == 0)
        queue.mTasksLock.unlock();

    return true;
}

void RuntimeQueue::AwaitTask(const ThreadId& targetThread, u64 taskId, rqe& ec)
{
    if(ThreadId().hash() == targetThread.hash())
        return;

    auto queue = context->queues.find(targetThread.hash());

    /* If thread has no queue, return */
    if(queue == context->queues.end())
    {
        ec = RQE::InvalidQueue;
        return;
    }

    auto const& queueRef = queue->second;

    RuntimeTask const* task = nullptr;
    szptr              idx  = 0;

    if(!(task = GetTask(queueRef.mTasks, taskId, ec, &idx)))
    {
        ec = RQE::InvalidTaskId;
        return;
    }

    /* We cannot reliably await periodic tasks */
    if(task->flags & RuntimeTask::Periodic)
    {
        ec = RQE::IncompatibleTaskAwait;
        return;
    }

    /* Do not await a past event */
    if(!(task->flags & RuntimeTask::Immediate) &&
       RuntimeTask::clock::now() > task->time)
    {
        ec = RQE::ExpiredTaskDeadline;
        return;
    }

    CurrentThread::sleep_until(task->time);

    /* I know this is bad, but we must await the task */
    while(queueRef.mTasks[idx].alive)
        CurrentThread::yield();
}

void RuntimeQueue::TerminateThread(RuntimeQueue* thread, rqe& ec)
{
    if(!thread)
    {
        ec = RQE::InvalidQueue;
        return;
    }

    auto tid = thread->threadId().hash();

    context->queueFlags[tid]->running.store(false);
    context->queueThreads[tid].join();

    context->queueFlags.erase(tid);
    context->queueThreads.erase(tid);
}

void RuntimeQueue::TerminateThreads(rqe& ec)
{
    C_UNUSED(ec);

    Lock _(context->globalMod);

    for(auto t : context->queueFlags)
    {
        t.second->running.store(false);
        t.second->condition.notify_one();
    }

    for(auto& t : context->queueThreads)
    {
        t.second.join();
    }

    context->queueFlags.clear();
    context->queueThreads.clear();
    context->queues.clear();
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
    RecLock _(mTasksLock);
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

RuntimeTask::Duration RuntimeQueue::timeTillNext()
{
    RuntimeTask::Timepoint current = RuntimeTask::clock::now();
    return timeTillNext(current);
}

RuntimeTask::Duration RuntimeQueue::timeTillNext(RuntimeTask::Timepoint current)
{
    RecLock _(mTasksLock);

    if(mTasks.size() == 0)
        return Chrono::milliseconds::max();
    else
    {
        if(mTasks[0].task.time < current)
            return Chrono::milliseconds(0);
        else
            return mTasks[0].task.time - current;
    }
}

CString RuntimeQueue::name()
{
    RecLock _(mTasksLock);
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
    RecLock _(mTasksLock);

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
