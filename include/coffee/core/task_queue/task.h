#pragma once

#include <coffee/core/types/tdef/stlfunctypes.h>
#include <coffee/core/plat/plat_timing.h>

namespace Coffee{

struct RuntimeTask
{
    enum TaskFlags
    {
        Parallel = 0x1,
        SingleShot = 0x2,
        Periodic = 0x4,
        Immediate = 0x8,
    };

    using clock = std::chrono::steady_clock;

    using Timepoint = std::chrono::time_point<clock>;
    using Duration = clock::duration;

    STATICINLINE RuntimeTask CreateTask(Function<void()>&& fun,
                                        TaskFlags f = SingleShot,
                                        Timepoint ts = {})
    {
        RuntimeTask task;
        task.flags = f;
        task.task = fun;
        task.time = ts;

        return task;
    }

    Function<void()> task;
    Timepoint time;
    Duration interval;
    TaskFlags flags;
    u32 _pad;

    FORCEDINLINE bool operator<(RuntimeTask const& other) const
    {
        return time < other.time;
    }
};

C_FLAGS(RuntimeTask::TaskFlags, u32);

class RuntimeQueue
{
public:
    static RuntimeQueue *CreateNewQueue(CString const& name);
    static RuntimeQueue *CreateNewThreadQueue(CString const& name);

    static RuntimeQueue *GetCurrentQueue();

    /*!
     * \brief From inside a task, get a reference to itself
     * \return
     */
    static RuntimeTask *GetSelf();

    /*!
     * \brief From inside a task, get its ID. Calling from outside will return 0.
     * \return
     */
    static u64 GetSelfId();

    /*!
     * \brief Enqueue a task into the current thread's runtime queue
     * For SingleShot tasks, if the time in the future is in the past
     *  upon validation, the task will be killed before queueing.
     * This behavior is so to keep deadlines.
     * Periodic tasks will simply be re-scheduled.
     * By adding ::Immediate flag to a task, deadlines will be "ignored",
     *  allowing a task to be executed even if delayed.
     * \param task Task to be run with its internal parameters
     * \return 0 on failure, non-zero value handle otherwise.
     */
    static u64 Queue(RuntimeTask&& task);
    /*!
     * \brief Like Queue(RuntimeTask&&), but targets a thread.
     * Queue(RuntimeTask&&) uses this function behind the scenes.
     * Same semantics apply.
     * \param targetThread
     * \param task
     * \return
     */
    static u64 Queue(ThreadId const& targetThread, RuntimeTask&& task);

    /*!
     * \brief Queue a single-shot task, without the effort
     * \param q Target queue, must be a valid, non-null queue
     * \param time Targeted time in the future
     * \param task Task to be run
     * \return
     */
    STATICINLINE u64 QueueShot(RuntimeQueue* q,
                               RuntimeTask::Duration time,
                               Function<void()>&& task)
    {
        if(!q)
            return 0;
        return Queue(q->threadId(),
                     RuntimeTask::CreateTask(
                         std::move(task),
                         RuntimeTask::SingleShot,
                         RuntimeTask::clock::now() + time
                         ));
    }

    /*!
     * \brief Like QueueShot, but will run as soon as possible. The difference is that this one ignores any possible deadlines. (QueueShot will kill the task if the deadline is not met)
     * \param q
     * \param task
     * \return
     */
    STATICINLINE u64 QueueImmediate(RuntimeQueue* q,
                                    RuntimeTask::Duration time,
                                    Function<void()>&& task)
    {
        if(!q)
            return 0;
        return Queue(q->threadId(),
                     RuntimeTask::CreateTask(
                         std::move(task),
                         RuntimeTask::SingleShot|RuntimeTask::Immediate,
                         RuntimeTask::clock::now() + time
                         ));
    }

    STATICINLINE u64 QueuePeriodic(RuntimeQueue* q,
                                   RuntimeTask::Duration time,
                                   Function<void()>&& task)
    {
        if(!q)
            return 0;
        return Queue(q->threadId(), {
                         task,
                         {},
                         time,
                         RuntimeTask::Periodic,
                         0,
                     });
    }

    struct task_data_t
    {
        RuntimeTask task;
        u64 index;
        bool alive;

        FORCEDINLINE bool operator<(task_data_t const& other) const
        {
            return task < other.task;
        }
    };

    static bool CancelTask(u64 taskId);
    static bool CancelTask(ThreadId const& targetThread, u64 taskId);

    static void AwaitTask(u64 taskId);
    static void AwaitTask(ThreadId const& targetThread, u64 taskId);

    static void TerminateThread(RuntimeQueue* thread);
    static void TerminateThreads();

    void executeTasks();
    RuntimeTask::Duration timeTillNext(RuntimeTask::Duration fallback);
    CString name();
    ThreadId threadId();

    RuntimeQueue();
    RuntimeQueue(const RuntimeQueue &queue);


private:

    u64 enqueue(RuntimeTask&& task);
    void sortTasks();


    Vector<task_data_t> mTasks;
    Mutex mTasksLock;
    u64 mTaskIndex;
    ThreadId mThreadId;

    u64 mCurrentTaskId;

    static Mutex globalMod;
    static Map<ThreadId::Hash, RuntimeQueue> queues;
    static Map<ThreadId::Hash, Thread> queueThreads;
    static Map<ThreadId::Hash, AtomicBool*> queueFlags;
};

}
