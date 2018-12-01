#pragma once

#include <coffee/core/libc_types.h>
#include <coffee/core/stl_types.h>
#include <peripherals/enum/helpers.h>
#include <peripherals/stl/functional_types.h>
#include <peripherals/stl/time_types.h>
#include <peripherals/stl/thread_types.h>

namespace Coffee {

enum class RuntimeQueueError
{
    ThreadSpawn = 1,   /*!< Upon failing to spawn a task thread */
    UncaughtException, /*!< Upon a thread dying from uncaught exception, it hits
                        a breakpad. Does not stop SIGSEGV from killing the
                        process. */
    ExpiredTaskDeadline, /*!< When a submitted SingleShot task is scheduled to
                            run at an expired time */
    InvalidTaskLifetime, /*!< Upon attempting to wait for a ::Periodic task */
    InvalidTaskFlags,    /*!< Upon attempting to ::Queue() a task which has an
                            invalid combination of flags */
    InvalidTaskId, /*!< When functions taking task IDs cannot find a task */
    IncompatibleTaskAwait, /*!< If ::Await() is called on a task that is
                              periodic, this is triggered */
    TaskAlreadyBlocked,    /*!< If ::Block() is called on a task that is already
                              blocked */
    TaskAlreadyStarted, /*!< If ::Unblock() is called on a task that is already
                           running */
    InvalidQueue, /*!< When any function is given a ThreadId which does not
                     point to a running RuntimeQueue */

    SameThread, /*!< Function has no effect on same thread */

    ShuttingDown /*!< Queues are shutting down, killing threads */
};

struct runtime_queue_category : error_category
{
    virtual const char* name() const noexcept;
    virtual std::string message(int error_code) const;
};

using runtime_queue_error =
    domain_error_code<RuntimeQueueError, runtime_queue_category>;

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
        task.flags       = f;
        task.task        = std::move(fun);
        task.time        = ts;

        return task;
    }

    STATICINLINE RuntimeTask CreateTask(
        Function<void()>&& fun, TaskFlags f = SingleShot, Duration d = {})
    {
        RuntimeTask task = {};
        task.flags       = f;
        task.task        = std::move(fun);
        task.interval    = d;

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
    struct semaphore_t
    {
        CondVar     condition;
        Mutex       start_mutex;
        Mutex       mutex;
        atomic_bool running;
    };

    struct QueueContext
    {
        QueueContext()
        {
            shutdownFlag.store(false);
        }
        ~QueueContext()
        {
            runtime_queue_error rqec;
            RuntimeQueue::TerminateThreads(rqec);
        }

        Mutex       globalMod;
        atomic_bool shutdownFlag;

        Map<ThreadId::Hash, RuntimeQueue> queues;
        Map<ThreadId::Hash, Thread>       queueThreads;
        /*!
         * \brief Contains all data necessary to manage a worker thread.
         * Needs to be a ShPtr<T> in order to avoid early destruction.
         */
        Map<ThreadId::Hash, ShPtr<semaphore_t>> queueFlags;
    };

    using QueueContextPtr = ShPtr<QueueContext>;
    using rqe             = runtime_queue_error;

    static RuntimeQueue* CreateNewQueue(CString const& name);
    static RuntimeQueue* CreateNewThreadQueue(CString const& name, rqe& ec);

    static RuntimeQueue* GetCurrentQueue(rqe& ec);

    /*!
     * \brief From inside a task, get a reference to itself
     * \return
     */
    static RuntimeTask* GetSelf(rqe& ec);

    /*!
     * \brief From inside a task, get its ID. Calling from outside will return
     * 0. \return
     */
    static u64 GetSelfId(rqe& ec);

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
    static u64 Queue(RuntimeTask&& task, rqe& ec);
    /*!
     * \brief Like Queue(RuntimeTask&&), but targets a thread.
     * Queue(RuntimeTask&&) uses this function behind the scenes.
     * Same semantics apply.
     * \param targetThread
     * \param task
     * \return
     */
    static u64 Queue(ThreadId const& targetThread, RuntimeTask&& task, rqe& ec);

    /*!
     * \brief Queue
     * \param queue
     * \param task
     * \param ec
     * \return
     */
    static u64 Queue(RuntimeQueue* queue, RuntimeTask&& task, rqe& ec);

    /*!
     * \brief Queue a single-shot task, without the effort
     * \param q Target queue, must be a valid, non-null queue
     * \param time Targeted time in the future
     * \param task Task to be run
     * \return
     */
    STATICINLINE u64 QueueShot(
        RuntimeQueue*         q,
        RuntimeTask::Duration time,
        Function<void()>&&    task,
        rqe&                  ec)
    {
        if(!q)
            return 0;
        return Queue(
            q->threadId(),
            RuntimeTask::CreateTask(
                std::move(task),
                RuntimeTask::SingleShot,
                RuntimeTask::clock::now() + time),
            ec);
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
        RuntimeQueue* q, Function<void()>&& task, rqe& ec, bool await = false)
    {
        if(!q)
        {
            ec = RuntimeQueueError::InvalidQueue;
            return 0;
        }

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
                RuntimeTask::clock::now()),
            ec);

        if(await)
            AwaitTask(q->threadId(), id, ec);

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
        RuntimeQueue*         q,
        RuntimeTask::Duration time,
        Function<void()>&&    task,
        rqe&                  ec)
    {
        if(!q)
            return 0;
        return Queue(
            q->threadId(),
            RuntimeTask::CreateTask(
                std::move(task),
                RuntimeTask::SingleShot | RuntimeTask::Immediate,
                RuntimeTask::clock::now() + time),
            ec);
    }

    STATICINLINE u64 QueuePeriodic(
        RuntimeQueue*         q,
        RuntimeTask::Duration time,
        Function<void()>&&    task,
        rqe&                  ec)
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
            },
            ec);
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

        u8 _pad[7];

        FORCEDINLINE bool operator<(task_data_t const& other) const
        {
            return task < other.task;
        }
    };

    STATICINLINE bool Block(u64 taskId, rqe& ec)
    {
        return Block(ThreadId(), taskId, ec);
    }
    static bool Block(ThreadId const& targetThread, u64 taskId, rqe& ec);

    STATICINLINE bool Unblock(u64 taskId, rqe& ec)
    {
        return Unblock(ThreadId(), taskId, ec);
    }
    static bool Unblock(ThreadId const& targetThread, u64 taskId, rqe& ec);

    STATICINLINE bool CancelTask(u64 taskId, rqe& ec)
    {
        return CancelTask(ThreadId(), taskId, ec);
    }
    static bool CancelTask(ThreadId const& targetThread, u64 taskId, rqe& ec);

    static void AwaitTask(ThreadId const& targetThread, u64 taskId, rqe& ec);

    static void TerminateThread(RuntimeQueue* thread, rqe& ec);
    static void TerminateThreads(rqe& ec);

    void                  executeTasks();
    RuntimeTask::Duration timeTillNext();
    RuntimeTask::Duration timeTillNext(RuntimeTask::Timepoint clock);
    CString               name();
    ThreadId              threadId();

    RuntimeQueue();
    RuntimeQueue(const RuntimeQueue& queue);

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

    RuntimeTask::Timepoint mNextWakeup;
    RecMutex               mTasksLock;

  private:
    u64  enqueue(RuntimeTask&& task);
    void sortTasks();

    Vector<task_data_t> mTasks;
    u64                 mTaskIndex;
    ThreadId            mThreadId;

    u64 mCurrentTaskId;

    static ShPtr<QueueContext> context;
};

struct ScopedTask
{
    ScopedTask() : m_id(0)
    {
    }

    template<typename... Args>
    ScopedTask(ThreadId const& tid, Function<void()>&& fun, Args... args)
    {
        runtime_queue_error ec;
        m_id = RuntimeQueue::Queue(
            tid, RuntimeTask::CreateTask(std::move(fun), args...), ec);

        C_ERROR_CHECK(ec);

        m_threadId = tid;
    }

    ScopedTask(ThreadId const& tid, RuntimeTask&& task)
    {
        runtime_queue_error ec;
        m_id = RuntimeQueue::Queue(tid, std::move(task), ec);

        C_ERROR_CHECK(ec);

        m_threadId = tid;
    }

    ScopedTask(ScopedTask&& other) :
        m_id(other.m_id), m_threadId(other.m_threadId)
    {
        other.m_id = 0;
    }

    C_DELETE_COPY_CONSTRUCTOR(ScopedTask);

    ~ScopedTask()
    {
        runtime_queue_error ec;
        if(m_id != 0)
            RuntimeQueue::CancelTask(m_threadId, m_id, ec);
    }

    ScopedTask& operator=(ScopedTask&& other)
    {
        m_id       = other.m_id;
        m_threadId = other.m_threadId;

        other.m_id       = 0;
        other.m_threadId = {};

        return *this;
    }

    u64 id() const
    {
        return m_id;
    }
    ThreadId threadId() const
    {
        return m_threadId;
    }

  private:
    u64      m_id;
    ThreadId m_threadId;
};

} // namespace Coffee
