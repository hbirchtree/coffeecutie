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

    bool operator<(RuntimeTask const& other) const
    {
        return time < other.time;
    }
};

class RuntimeQueue
{
public:
    static RuntimeQueue *CreateNewQueue(CString const& name);
    static RuntimeQueue *CreateNewThreadQueue(CString const& name);

    /*!
     * \brief Enqueue a task into the current thread's runtime queue
     * \param task Task to be run with its internal parameters
     * \return 0 on failure, non-zero value handle otherwise.
     */
    static u64 Queue(RuntimeTask&& task);
    static u64 Queue(ThreadId const& targetThread, RuntimeTask&& task);

    static bool CancelTask(u64 taskId);
    static bool CancelTask(ThreadId const& targetThread, u64 taskId);

    static void AwaitTask(u64 taskId){}
    static void AwaitTask(ThreadId const& targetThread, u64 taskId){}

    void executeTasks();
    CString name();

    RuntimeQueue();
    RuntimeQueue(const RuntimeQueue &queue);

private:

    u64 enqueue(RuntimeTask&& task);
    void sortTasks();

    Vector<RuntimeTask> mTasks;
    Vector<bool> mTasksAlive;
    Mutex mTasksLock;
    u64 mTaskIndex;

    static Mutex globalMod;
    static Map<ThreadId::Hash, RuntimeQueue> queues;
    static Map<ThreadId::Hash, Thread> queueThreads;
};

}
