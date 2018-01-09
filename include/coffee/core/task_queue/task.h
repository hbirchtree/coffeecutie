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
     * \param task Task to be run with its internal parameters
     * \return 0 on failure, non-zero value handle otherwise.
     */
    static u64 Queue(RuntimeTask&& task);
    static u64 Queue(ThreadId const& targetThread, RuntimeTask&& task);

    STATICINLINE u64 QueueShot(RuntimeQueue* q,
                               RuntimeTask::Duration time,
                               Function<void()>&& task)
    {
        return Queue(q->threadId(),
                     RuntimeTask::CreateTask(
                         std::move(task),
                         RuntimeTask::SingleShot,
                         RuntimeTask::clock::now() + time
                         ));
    }

    STATICINLINE u64 QueuePeriodic(RuntimeQueue* q,
                                   RuntimeTask::Duration time,
                                   Function<void()>&& task)
    {
        return Queue(q->threadId(), {
                         task,
                         {},
                         time,
                         RuntimeTask::Periodic,
                         0,
                     });
    }

    static bool CancelTask(u64 taskId);
    static bool CancelTask(ThreadId const& targetThread, u64 taskId);

    static void AwaitTask(u64 taskId);
    static void AwaitTask(ThreadId const& targetThread, u64 taskId);

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

    Vector<RuntimeTask> mTasks;
    Vector<u64> mTaskIndices;
    Vector<bool> mTasksAlive;
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
