#pragma once

#include <coffee/core/types/edef/enumfun.h>
#include <coffee/core/types/tdef/stlfunctypes.h>

namespace Coffee {

struct RuntimeTask
{
    enum TaskFlags
    {
        Parallel   = 0x1,
        SingleShot = 0x2,
        Periodic   = 0x4,
        Immediate  = 0x8,
    };

    using clock = Chrono::steady_clock;

    using Timepoint = Chrono::time_point<clock>;
    using Duration  = clock::duration;

    STATICINLINE RuntimeTask CreateTask(
        Function<void()>&& fun, TaskFlags f = SingleShot, Timepoint ts = {})
    {
        RuntimeTask task = {};
        task.flags = f;
        task.task  = std::move(fun);
        task.time  = ts;

        return task;
    }

    STATICINLINE RuntimeTask CreateTask(
            Function<void()>&& fun, TaskFlags f = SingleShot, Duration d = {})
    {
        RuntimeTask task = {};
        task.flags = f;
        task.task = std::move(fun);
        task.interval = d;

        return task;
    }

    Function<void()> task;
    Timepoint        time;
    Duration         interval;
    TaskFlags        flags;
    u32              _pad;

    FORCEDINLINE bool operator<(RuntimeTask const& other) const
    {
        return time < other.time;
    }
};

C_FLAGS(RuntimeTask::TaskFlags, u32);

class RuntimeQueue
{
  public:
    static RuntimeQueue* CreateNewQueue(CString const& name);
    static RuntimeQueue* CreateNewThreadQueue(CString const& name);

    static RuntimeQueue* GetCurrentQueue();

    /*!
     * \brief From inside a task, get a reference to itself
     * \return
     */
    static RuntimeTask* GetSelf();

    /*!
     * \brief From inside a task, get its ID. Calling from outside will return
     * 0. \return
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
    STATICINLINE u64 QueueShot(
        RuntimeQueue* q, RuntimeTask::Duration time, Function<void()>&& task)
    {
        if(!q)
            return 0;
        return Queue(
            q->threadId(),
            RuntimeTask::CreateTask(
                std::move(task),
                RuntimeTask::SingleShot,
                RuntimeTask::clock::now() + time));
    }

    /*!
     * \brief For running a task on a thread, running it
     *  immediately if possible.
     * This is very handy for thread-bound contexts.
     * \param q
     * \param task
     * \return
     */
    STATICINLINE u64 QueueEnsureThread(
        RuntimeQueue* q, Function<void()>&& task, bool await = false)
    {
        if(!q)
            return 0;

        /* If we are on the desired thread, run it now */
        if(ThreadId() == q->threadId())
        {
            task();
            return 0;
        }

        /* Otherwise queue it */
        u64 id = Queue(
            q->threadId(),
            RuntimeTask::CreateTask(
                std::move(task),
                RuntimeTask::SingleShot | RuntimeTask::Immediate,
                RuntimeTask::clock::now()));

        if(await)
            AwaitTask(q->threadId(), id);

        return id;
    }

    /*!
     * \brief Like QueueShot, but will run as soon as possible. The difference
     * is that this one ignores any possible deadlines. (QueueShot will kill the
     * task if the deadline is not met)
     * \param q
     * \param task
     * \return
     */
    STATICINLINE u64 QueueImmediate(
        RuntimeQueue* q, RuntimeTask::Duration time, Function<void()>&& task)
    {
        if(!q)
            return 0;
        return Queue(
            q->threadId(),
            RuntimeTask::CreateTask(
                std::move(task),
                RuntimeTask::SingleShot | RuntimeTask::Immediate,
                RuntimeTask::clock::now() + time));
    }

    STATICINLINE u64 QueuePeriodic(
        RuntimeQueue* q, RuntimeTask::Duration time, Function<void()>&& task)
    {
        if(!q)
            return 0;
        return Queue(
            q->threadId(),
            {
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
        u64         index;
        union
        {
            struct
            {
                bool alive : 1;
                bool to_dispose : 1;
            };
        };

        FORCEDINLINE bool operator<(task_data_t const& other) const
        {
            return task < other.task;
        }
    };

    static bool Block(u64 taskId);
    static bool Block(ThreadId const& targetThread, u64 taskId);

    static bool Unblock(u64 taskId);
    static bool Unblock(ThreadId const& targetThread, u64 taskId);

    static bool CancelTask(u64 taskId);
    static bool CancelTask(ThreadId const& targetThread, u64 taskId);

    static void AwaitTask(u64 taskId);
    static void AwaitTask(ThreadId const& targetThread, u64 taskId);

    static void TerminateThread(RuntimeQueue* thread);
    static void TerminateThreads();

    void                  executeTasks();
    RuntimeTask::Duration timeTillNext(RuntimeTask::Duration fallback);
    CString               name();
    ThreadId              threadId();

    RuntimeQueue();
    RuntimeQueue(const RuntimeQueue& queue);

    struct QueueContext
    {
        Mutex                             globalMod;
        Map<ThreadId::Hash, RuntimeQueue> queues;
        Map<ThreadId::Hash, Thread>       queueThreads;
        Map<ThreadId::Hash, AtomicBool*>  queueFlags;
    };

    using QueueContextPtr = ShPtr<QueueContext>;

    /*!
     * \brief Create a new thread context, do nothing else.
     * \return
     */
    static QueueContextPtr CreateContext();
    /*!
     * \brief Set queue context if and only if there is none from before.
     * If one already exists, we cannot control it.
     * \param i
     */
    static bool SetQueueContext(QueueContextPtr i);
    /*!
     * \brief Return the current context, regardless of whether it exists.
     * \return
     */
    static QueueContextPtr GetQueueContext();

  private:
    u64  enqueue(RuntimeTask&& task);
    void sortTasks();

    Vector<task_data_t> mTasks;
    Mutex               mTasksLock;
    u64                 mTaskIndex;
    ThreadId            mThreadId;

    u64 mCurrentTaskId;

    static ShPtr<QueueContext> context;
};

struct ScopedTask
{
    template<typename... Args>
    ScopedTask(ThreadId const& tid, Function<void()>&& fun, Args... args)
    {
        m_id = RuntimeQueue::Queue(
            tid, RuntimeTask::CreateTask(std::move(fun), args...));
        m_threadId = tid;
    }

    ScopedTask(ThreadId const& tid, RuntimeTask&& task)
    {
        m_id = RuntimeQueue::Queue(tid, std::move(task));
        m_threadId = tid;
    }

    ScopedTask(ScopedTask&& other):
        m_id(other.m_id),
        m_threadId(other.m_threadId)
    {
        other.m_id = 0;
    }

    C_DELETE_COPY_CONSTRUCTOR(ScopedTask);

    ~ScopedTask()
    {
        if(m_id != 0)
            RuntimeQueue::CancelTask(m_threadId, m_id);
    }

    u64 id() const
    {
        return m_id;
    }

private:
    u64 m_id;
    ThreadId m_threadId;
};

} // namespace Coffee
