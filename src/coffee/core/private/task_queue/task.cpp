#include <coffee/core/task_queue/task.h>

#include <algorithm>
#include <coffee/core/CProfiling>
#include <coffee/core/internal_state.h>
#include <platforms/stacktrace.h>

#include <coffee/strings/libc_types.h>

#include <coffee/core/CDebug>

#define RQ_API "RuntimeQueue::"

using Stacktracer = platform::env::Stacktracer;

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
    case RQE::SameThread:
        return "Cannot be performed on same thread";
    case RQE::ShuttingDown:
        return "Operation unavailable on shutdown";
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
    } else
        Profiler::DeepProfile(RQ_API "Skipping thread");
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
        CurrentThread::SetName(name);
        return &context->queues[t_id];
    } else
        return &(*q_it).second;
}

STATICINLINE void ThreadQueueSleep(
    RuntimeQueue* queue, UqLock& thread_lock, RuntimeQueue::semaphore_t* sem_)
{
    if(!sem_->running)
        return;

    DProfContext ___(RQ_API "Sleeping");

    RuntimeTask::Duration sleepTime;

    {
        RecLock _(queue->mTasksLock);
        auto    currentTime = RuntimeTask::clock::now();

        sleepTime          = queue->timeTillNext(currentTime);
        queue->mNextWakeup = currentTime + sleepTime;
    }

    sem_->condition.wait_for(thread_lock, sleepTime);
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

        {
            DProfContext _(RQ_API "Running queue");
            while(sem_->running)
            {
                queue->executeTasks();

                ThreadQueueSleep(queue, thread_lock, sem_.get());
            }
        }

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
                   "{0}: {1}"
                   "\n",
            ec.message(),
            ec.error_message);
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

    if(!context->globalMod.try_lock())
    {
        ec = RQE::ShuttingDown;
        return 0;
    }

    auto thread_id = targetThread.hash();
    auto q_it      = context->queues.find(thread_id);

    if(q_it == context->queues.end())
    {
        context->globalMod.unlock();
        ec = RQE::InvalidQueue;
        return 0;
    } else
    {
        context->globalMod.unlock();
        return Queue(&q_it->second, std::move(task), ec);
    }
}

u64 RuntimeQueue::Queue(
    RuntimeQueue* queue, RuntimeTask&& task, RuntimeQueue::rqe& ec)
{
    if(context->shutdownFlag.load())
    {
        ec = RQE::ShuttingDown;
        return 0;
    }

    DProfContext _(RQ_API "Adding task to Queue");
    auto&        ref = *queue;

    RecLock __(queue->mTasksLock);

    auto currentBase      = RuntimeTask::clock::now();
    auto previousNextTime = ref.timeTillNext(currentBase);

    auto id = ref.enqueue(std::move(task));

    NotifyThread(
        context, queue->mThreadId.hash(), previousNextTime, currentBase);

    return id;
}

bool RuntimeQueue::Block(const ThreadId& targetThread, u64 taskId, rqe& ec)
{
    if(context->shutdownFlag.load())
    {
        ec = RQE::ShuttingDown;
        return false;
    }

    DProfContext __(RQ_API "Blocking task");

    auto          thread_id = targetThread.hash();
    RuntimeQueue* pQueue    = nullptr;

    {
        Lock _(context->globalMod);

        auto q_it = context->queues.find(thread_id);

        if(q_it == context->queues.end())
        {
            ec = RQE::InvalidQueue;
            return false;
        }

        pQueue = &(*q_it).second;
    }

    auto& queue = *pQueue;

    RuntimeTask const* task = nullptr;
    szptr              idx  = 0;

    if(!(task = GetTask(queue.mTasks, taskId, ec, &idx)))
    {
        ec = RQE::InvalidTaskId;
        return false;
    }

    {
        /* We do this check in case we are executing in the queue */
        /* Otherwise we deadlock */
        RecLock ___(queue.mTasksLock);

        if(!queue.mTasks[idx].alive)
        {
            ec = RQE::TaskAlreadyBlocked;
            return false;
        }

        auto currentBase      = RuntimeTask::clock::now();
        auto previousNextTime = queue.timeTillNext(currentBase);

        queue.mTasks[idx].alive = false;

        NotifyThread(context, thread_id, previousNextTime, currentBase);
    }

    return true;
}

bool RuntimeQueue::Unblock(const ThreadId& targetThread, u64 taskId, rqe& ec)
{
    if(context->shutdownFlag.load())
    {
        ec = RQE::ShuttingDown;
        return false;
    }

    DProfContext __(RQ_API "Unblocking task");

    auto          thread_id = targetThread.hash();
    RuntimeQueue* pQueue    = nullptr;

    {
        Lock _(context->globalMod);
        auto q_it = context->queues.find(thread_id);

        if(q_it == context->queues.end())
        {
            ec = RQE::InvalidQueue;
            return false;
        }

        pQueue = &(*q_it).second;
    }

    auto& queue = *pQueue;

    {
        RecLock            _(queue.mTasksLock);
        RuntimeTask const* task = nullptr;
        szptr              idx  = 0;

        if(!(task = GetTask(queue.mTasks, taskId, ec, &idx)))
        {
            ec = RQE::InvalidTaskId;
            return false;
        }

        if(queue.mTasks[idx].alive)
        {
            ec = RQE::TaskAlreadyStarted;
            return false;
        }

        auto currentBase      = RuntimeTask::clock::now();
        auto previousNextTime = queue.timeTillNext(currentBase);

        queue.mTasks[idx].alive = true;

        NotifyThread(context, thread_id, previousNextTime, currentBase);
    }

    return true;
}

bool RuntimeQueue::CancelTask(const ThreadId& targetThread, u64 taskId, rqe& ec)
{
    if(context->shutdownFlag.load())
    {
        ec = RQE::ShuttingDown;
        return false;
    }

    auto          thread_id = targetThread.hash();
    RuntimeQueue* pQueue    = nullptr;

    {
        Lock _(context->globalMod);

        auto q_it = context->queues.find(thread_id);

        if(q_it == context->queues.end())
        {
            ec = RQE::InvalidQueue;
            return false;
        }

        pQueue = &(*q_it).second;
    }

    auto& queue = *pQueue;

    {
        RecLock _(queue.mTasksLock);

        RuntimeTask const* task = nullptr;
        szptr              idx  = 0;

        if(!(task = GetTask(queue.mTasks, taskId, ec, &idx)))
        {
            ec = RQE::InvalidTaskId;
            return false;
        }

        queue.mTasks[idx].alive      = false;
        queue.mTasks[idx].to_dispose = true;

        auto currentBase      = RuntimeTask::clock::now();
        auto previousNextTime = queue.timeTillNext(currentBase);

        NotifyThread(context, thread_id, previousNextTime, currentBase);
    }

    return true;
}

void RuntimeQueue::AwaitTask(const ThreadId& targetThread, u64 taskId, rqe& ec)
{
    if(taskId == 0)
        return;

    if(context->shutdownFlag.load())
    {
        ec = RQE::ShuttingDown;
        return;
    }

    if(ThreadId().hash() == targetThread.hash())
    {
        ec = RQE::SameThread;
        return;
    }

    RuntimeQueue const* queueRef = nullptr;

    {
        Lock _(context->globalMod);

        auto queue = context->queues.find(targetThread.hash());

        /* If thread has no queue, return */
        if(queue == context->queues.end())
        {
            ec = RQE::InvalidQueue;
            return;
        }

        queueRef = &queue->second;
    }

    RuntimeTask const* task = nullptr;
    szptr              idx  = 0;

    if(!(task = GetTask(queueRef->mTasks, taskId, ec, &idx)))
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

    {
        DProfContext _(RQ_API "Busy-waiting task");

        /* I know this is bad, but we must await the task */
        while(queueRef->mTasks[idx].alive)
            CurrentThread::yield();
    }
}

void RuntimeQueue::TerminateThread(RuntimeQueue* thread, rqe& ec)
{
    if(!thread)
    {
        ec = RQE::InvalidQueue;
        return;
    }

    auto tid = thread->threadId().hash();

    Lock _(context->globalMod);

    auto& queueFlags = context->queueFlags[tid];

    queueFlags->running.store(false);
    queueFlags->condition.notify_one();

    context->queueThreads[tid].join();

    context->queueFlags.erase(tid);
    context->queueThreads.erase(tid);
}

void RuntimeQueue::TerminateThreads(rqe&)
{
    context->shutdownFlag.store(true);
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
        {
            mTasks[i].alive      = false;
            mTasks[i].to_dispose = true;
        } else if(task.task.flags & RuntimeTask::Periodic)
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
    task_data_t* firstTask = nullptr;

    for(auto task : mTasks)
        if(task.alive)
        {
            firstTask = &task;
            break;
        }

    if(!firstTask)
    {
        Profiler::DeepProfile(RQ_API "Entering deep sleep");
        if(mNextWakeup < current)
            return Chrono::seconds(10);
        else
            return mNextWakeup - current;
    } else
    {
        if(firstTask->task.time < current)
            return Chrono::milliseconds::zero();
        else
            return firstTask->task.time - current;
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
    mTasksLock(), mTasks(), mTaskIndex(0), mCurrentTaskId(0)
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
